#include <SparkFun_I2C_GPS_Arduino_Library.h>
#include <Adafruit_EPD.h>

#include <TMC2209.h>
#include <BasicStepperDriver.h>
#include <VL53L0X.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <FS.h>
#include <FFat.h>
#include <USB.h>
#include <USBMSC.h>
#include <FirmwareMSC.h>
#include <ArduinoJson.h>

#include <esp_partition.h>

#define FFAT_SECTOR_SIZE 4096

#define STP_MS1   17
#define STP_MS2   18
#define STP_MS3   33
#define STP_PDN   34
#define STP_DIAG  35
#define STP_EN    38
#define STP_STEP  39
#define STP_DIR   40

#define STP_UART_TX  STP_MS3
#define STP_UART_RX  STP_PDN

#define EPD_DC    14
#define EPD_CS    10
#define EPD_BUSY  16
#define SRAM_CS   -1
#define EPD_RESET 15
#define EPD_SPI   &SPI

#define TOF_XSHUT 7

#define GNSS_INT  6

#define HALL      41

#define STEPPER_MICROSTEPS      64
#define STEPPER_STEPS_PER_REV   750
#define RUN_CURRENT_PERCENT     100
#define HOLD_CURRENT_PERCENT    100
#define STALL_GUARD_THRESHOLD   1

#define SDA_PIN                 8
#define SCL_PIN                 9
#define RGB_LED_PIN             21

#define BOARD_ID_LENGTH         16
#define BOARD_NAME_LENGTH       32

#define WIFI_SSID_LENGTH        32
#define WIFI_PASSWORD_LENGTH    32

#define ANT_DETECTION_THRESHHOLD 10

HardwareSerial      &serial_stream = Serial2;

TMC2209             stepperDriver;
BasicStepperDriver  stepperMotor(STEPPER_STEPS_PER_REV, STP_DIR, STP_STEP, STP_EN);
I2CGPS              gnssDevice;
VL53L0X             tofDevice;
Adafruit_SSD1608    epdDevice(200, 200, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);
FirmwareMSC         MSC_Firmware;
USBMSC              MSC_Config;


typedef struct
{
  char      board_id[BOARD_ID_LENGTH];
  uint16_t  tof_trigger_distance_mm;
  uint16_t  tof_nominal_distance_mm;
  uint16_t  tof_variance_mm;
  int8_t    wifi_rssi;
  bool      has_tof;
  bool      has_gnss;
} ackbar_calibration_data_t;

typedef struct
{
  char      board_name[BOARD_NAME_LENGTH];
  uint16_t  motor_rpm;
  uint16_t  motor_acceleration;
  uint16_t  motor_deceleration;
  uint32_t  tof_timing_budget_us;
  char      wifi_ssid[WIFI_SSID_LENGTH];
  char      wifi_password[WIFI_PASSWORD_LENGTH];
  uint32_t  wifi_timeout_ms;
  double    home_latitude;
  double    home_longitude;
  uint32_t  geofence_distance_m;
  bool      require_wifi;
  bool      require_gnss;
} ackbar_config_data_t;

typedef enum
{
  ACKBAR_STATE_RESET,
  ACKBAR_STATE_ERROR,
  ACKBAR_STATE_STARTING,
  ACKBAR_STATE_ARMING,
  ACKBAR_STATE_ARMED,
  ACKBAR_STATE_TRAPPING,
  ACKBAR_STATE_REPORTING,
  ACKBAR_STATE_LAST
} ackbar_state_t;

typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} ackbar_color_t;

typedef void (*ackbar_state_function_t)();

typedef struct
{
  ackbar_state_t          state;
  ackbar_state_function_t func;
  char                    description[16];
  ackbar_color_t          color;
} ackbar_state_table_entry_t;

//Forward declarations for the dispatch table
void state_reset();
void state_error();
void state_starting();
void state_arming();
void state_armed();
void state_trapping();
void state_reporting();

ackbar_state_table_entry_t dispatchTable[ACKBAR_STATE_LAST] =
{
  { ACKBAR_STATE_RESET,      &state_reset,     "RESET",     {64,   0,    255 } },
  { ACKBAR_STATE_ERROR,      &state_error,     "ERROR",     {255,  0,    0   } },
  { ACKBAR_STATE_STARTING,   &state_starting,  "STARTING",  {0,    0,    64  } },
  { ACKBAR_STATE_ARMING,     &state_arming,    "ARMING",    {64,   64,   0   } },
  { ACKBAR_STATE_ARMED,      &state_armed,     "ARMED",     {0,    64,   0   } },
  { ACKBAR_STATE_TRAPPING,   &state_trapping,  "TRAPPING",  {64,   64,   64  } },
  { ACKBAR_STATE_REPORTING,  &state_reporting, "REPORTING", {64,   0,    64  } }
};
  
ackbar_config_data_t        configData;
ackbar_calibration_data_t   calibrationData;
ackbar_state_t              currentState = ACKBAR_STATE_RESET;

void change_state(ackbar_state_t newState)
{
  for(int i = 0; i < ACKBAR_STATE_LAST; i++)
  {
    if(dispatchTable[i].state == newState)
    {
      ackbar_color_t * color = &dispatchTable[i].color;
      neopixelWrite(RGB_LED_PIN, color->green, color->red, color->blue);
      Serial.print("Global state is now: ");
      Serial.println(dispatchTable[i].description);
      break;
    }
  }

  currentState = newState;
}

void stepperDiagInterrupt()
{
  Serial.println("STALLGUARD!");
}

void init_motor()
{
  Serial.println("Initializing Motor");

  pinMode(STP_MS1, OUTPUT);
  pinMode(STP_MS2, OUTPUT);
  pinMode(STP_DIAG, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(STP_DIAG), &stepperDiagInterrupt, FALLING);


  digitalWrite(STP_MS1, LOW);
  digitalWrite(STP_MS2, LOW);

  Serial2.begin(115200, SERIAL_8N1, STP_UART_RX, STP_UART_TX);

  stepperDriver.setup(serial_stream);
  stepperDriver.enableAutomaticCurrentScaling();
  stepperDriver.setMicrostepsPerStep(STEPPER_MICROSTEPS);
  stepperDriver.setRunCurrent(RUN_CURRENT_PERCENT);
  stepperDriver.setHoldCurrent(HOLD_CURRENT_PERCENT);
  stepperDriver.setStandstillMode(stepperDriver.STRONG_BRAKING);
  stepperDriver.setStallGuardThreshold(STALL_GUARD_THRESHOLD);
  stepperDriver.disableCoolStep();
  stepperDriver.enable();

  stepperMotor.setEnableActiveState(LOW);
  stepperMotor.begin(configData.motor_rpm, STEPPER_MICROSTEPS);

  stepperMotor.setSpeedProfile(
    stepperMotor.LINEAR_SPEED, 
    configData.motor_acceleration * STEPPER_STEPS_PER_REV, 
    configData.motor_deceleration * STEPPER_STEPS_PER_REV
  );

  stepperMotor.disable();
}

void gnss_interrupt()
{
  Serial.println("GNSS interrupt fired!");
}

void init_gnss()
{
  pinMode(GNSS_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GNSS_INT), &gnss_interrupt, FALLING);

  calibrationData.has_gnss = false;  
}

void init_clock()
{
  if(calibrationData.has_gnss)
  {
    Serial.println("Setting clock from GNSS");
  }
  else
  {
    if( start_wifi() )
    {
      Serial.println("Setting clock via NTP");
      configTime(0, 0, "pool.ntp.org");

      stop_wifi();
    }
    else
    {
      Serial.println("!!! Failed to set clock via NTP");
    }
  }
}

void init_tof_sensor()
{
  Serial.println("Initializing ToF sensor");

  pinMode(TOF_XSHUT, OUTPUT);
  digitalWrite(TOF_XSHUT, LOW);
  delay(1);
  digitalWrite(TOF_XSHUT, HIGH);
  delay(10);

  tofDevice.setTimeout(500);

  if(tofDevice.init())
  {
    tofDevice.setMeasurementTimingBudget(configData.tof_timing_budget_us);
    calibrationData.has_tof = true;
  }
  else
  {
    Serial.println("!!! Failed to initialize ToF sensor");
    calibrationData.has_tof = false;
  }
}

void init_pins()
{
  Wire.begin(SDA_PIN, SCL_PIN);

  pinMode(HALL, INPUT_PULLUP);
}

bool start_wifi()
{
  if(strlen(configData.wifi_ssid) == 0)
  {
    Serial.println("WiFi is not configured");
    return 0;
  }

  Serial.println("Connecting to WiFi");

  int timeout_remaining = configData.wifi_timeout_ms;
  int timeout_at        = millis() + configData.wifi_timeout_ms;

  while(millis() < timeout_at)
  {
    WiFi.begin(configData.wifi_ssid, configData.wifi_password);

    int timeout_remaining = timeout_at - millis();

    int start             = millis();
    uint8_t status        = WiFi.waitForConnectResult(timeout_remaining);
    int elapsed           = millis() - start;

    Serial.print("Spent ");
    Serial.print(elapsed);
    Serial.println(" ms for WiFi connection result");

    if(status == WL_CONNECTED)
    {
      calibrationData.wifi_rssi = WiFi.RSSI();

      Serial.printf("Connected to WiFi: %s\n",      configData.wifi_ssid);
      Serial.printf("IP address:        %s\n",      WiFi.localIP().toString().c_str());
      Serial.printf("Signal Strength:   %i dBm\n",  calibrationData.wifi_rssi);

      return true;
    }
    else
    {
      Serial.printf("WiFi connection failed, wl_status_t code: %d", status);

      delay(50);

    }
  }
  return false;
}

void stop_wifi()
{
  Serial.println("Disconnecting from WiFi");
  WiFi.disconnect(true);
}

bool send_notification(char *message)
{
  bool success = false;

  if(start_wifi())
  {
    WiFiClient client;
    HTTPClient http;

    String uri = "http://ntfy.sh/RAT_TRAP_";
    uri += calibrationData.board_id;

    Serial.print("Notification URL: ");
    Serial.println(uri);

    http.begin(client, uri);

    http.addHeader("Content-Type", "text/plain");

    String request("Notification From ");
    request += configData.board_name;
    request += ": ";
    request += message;

    int response = http.POST(request);

    Serial.print("HTTP Response status code: ");
    Serial.println(response);

    if(response == 200)
    {
      success = true;
    }

    http.end();
    client.stop();
  }

  stop_wifi();

  return(success);
}

bool limit_switch_triggered(uint8_t pin)
{
  return (digitalRead(pin) == LOW);
}

void move_to_limit(uint8_t pin)
{
  //Serial.println("Finding limit switch...");
  while(1)
  {
    int count = 0;
    stepperMotor.enable();
    while((count < 360) && (limit_switch_triggered(pin) == false))
    {
      count++;
      stepperMotor.rotate(1);
    }
    stepperMotor.disable();

    if(limit_switch_triggered(pin) == true)
    {
      return;
    }
    else
    {
      Serial.println("Motor did not reach limit after commanded full rotation... waiting 60 seconds");
      delay(60000);
    }
  }

  //Serial.print("Hit limit switch after ");
  //Serial.print(count);
  //Serial.println(" degrees of rotation");
}

void flip()
{
  stepperMotor.enable();
  stepperMotor.rotate(180);
  stepperMotor.disable();
}

void calibrate()
{
  Serial.println("Calibrating");
  move_to_limit(HALL);
  flip();

  delay(100);

  uint16_t min_distance   = -1;
  uint16_t max_distance   = 0;
  uint32_t total_distance = 0;
  uint8_t samples         = 0;
  while(samples < 100)
  {
    uint16_t distance_mm = tofDevice.readRangeSingleMillimeters();

    if(tofDevice.timeoutOccurred())
    {
      Serial.println("Timeout");
    }
    else
    {
      total_distance += distance_mm;
      samples++;
      if(distance_mm > max_distance)
      {
        max_distance = distance_mm;
      }
      if(distance_mm < min_distance)
      {
        min_distance = distance_mm;
      }
      delay(10);
    }
  }

  uint16_t average_distance = (uint16_t)(total_distance / samples);
  calibrationData.tof_nominal_distance_mm = average_distance;

  uint16_t min_variance = average_distance - min_distance;
  uint16_t max_variance = max_distance - average_distance;

  if(min_variance > max_variance)
  {
    calibrationData.tof_variance_mm = min_variance;
  }
  else
  {
    calibrationData.tof_variance_mm = max_variance;
  }

  calibrationData.tof_trigger_distance_mm = max_variance * 2;

  Serial.print("Minimum Sensor Distance: ");
  Serial.print(min_distance);
  Serial.println(" mm");

  Serial.print("Maximum Sensor Distance: ");
  Serial.print(max_distance);
  Serial.println(" mm");

  Serial.print("Nominal Sensor Distance: ");
  Serial.print(calibrationData.tof_nominal_distance_mm);
  Serial.println(" mm");

  Serial.print("Sensor Variance: ");
  Serial.print(calibrationData.tof_variance_mm);
  Serial.println(" mm");

  Serial.print("Trigger Distance: ");
  Serial.print(calibrationData.tof_trigger_distance_mm);
  Serial.println(" mm");
}

void init_board_id()
{
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");
  macAddress.toCharArray(calibrationData.board_id,   BOARD_ID_LENGTH);

  Serial.print("Board ID: ");
  Serial.println(calibrationData.board_id);
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

struct PsramAllocator {
  void* allocate(size_t size)
  {
    return ps_malloc(size);
  }

  void deallocate(void* pointer)
  {
    free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size)
  {
    return ps_realloc(ptr, new_size);
  }
};

using PsramJsonDocument = BasicJsonDocument<PsramAllocator>;

void init_config_data()
{
  configData.motor_rpm                = 240;
  configData.motor_acceleration       = 8;
  configData.motor_deceleration       = 8;
  configData.tof_timing_budget_us     = 50000;
  configData.wifi_timeout_ms          = 60000;
  configData.require_wifi             = false;
  configData.require_gnss             = false;
  configData.home_latitude            = 0.0;
  configData.home_longitude           = 0.0;
  configData.geofence_distance_m      = 0;

  strcpy(configData.wifi_ssid,        "");
  strcpy(configData.wifi_password,    "");
  strcpy(configData.board_name,       "Unnamed");

  readFile(FFat, "/config.json");

  File config = FFat.open("/config.json", "r", false);

  if(! config)
  {
    Serial.println("Failed to open config.json");
    return;
  }

  PsramJsonDocument doc(1024);

  DeserializationError error = deserializeJson(doc, config);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());

    Serial.println(config.readString());

    return;
  }

  configData.motor_rpm              = doc["motor_rpm"];
  configData.motor_acceleration     = doc["motor_acceleration"];
  configData.motor_deceleration     = doc["motor_deceleration"];
  configData.tof_timing_budget_us   = doc["tof_timing_budget_us"];
  configData.wifi_timeout_ms        = doc["wifi_timeout_ms"];
  configData.home_latitude          = doc["home_latitude"];
  configData.home_longitude         = doc["home_longitude"];
  configData.geofence_distance_m    = doc["geofence_distance_m"];
  configData.require_wifi           = doc["require_wifi"];
  configData.require_gnss           = doc["require_gnss"];

  strncpy(configData.board_name,      doc["board_name"],     sizeof(configData.board_name));
  strncpy(configData.wifi_ssid,       doc["wifi_ssid"],      sizeof(configData.wifi_ssid));
  strncpy(configData.wifi_password,   doc["wifi_password"],  sizeof(configData.wifi_password));
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if(event_base == ARDUINO_USB_EVENTS)
  {
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        Serial.println("USB PLUGGED");
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        Serial.println("USB UNPLUGGED");
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        break;
      case ARDUINO_USB_RESUME_EVENT:
        Serial.println("USB RESUMED");
        break;
      
      default:
        break;
    }
  } else if(event_base == ARDUINO_FIRMWARE_MSC_EVENTS)
  {
    arduino_firmware_msc_event_data_t * data = (arduino_firmware_msc_event_data_t*)event_data;
    switch (event_id)
    {
      case ARDUINO_FIRMWARE_MSC_START_EVENT:
        Serial.println("MSC Update Start");
        break;
      case ARDUINO_FIRMWARE_MSC_WRITE_EVENT:
        //HWSerial.printf("MSC Update Write %u bytes at offset %u\n", data->write.size, data->write.offset);
        Serial.print(".");
        break;
      case ARDUINO_FIRMWARE_MSC_END_EVENT:
        Serial.printf("\nMSC Update End: %u bytes\n", data->end.size);
        break;
      case ARDUINO_FIRMWARE_MSC_ERROR_EVENT:
        Serial.printf("MSC Update ERROR! Progress: %u bytes\n", data->error.size);
        break;
      case ARDUINO_FIRMWARE_MSC_POWER_EVENT:
        Serial.printf("MSC Update Power: power: %u, start: %u, eject: %u", data->power.power_condition, data->power.start, data->power.load_eject);
        break;
      
      default:
        break;
    }
  }
}

esp_partition_t *ffat_partition = NULL;

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize){
  Serial.printf("MSC WRITE: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);

  size_t dst_offset = (lba * FFAT_SECTOR_SIZE) + offset;\
  esp_err_t status = esp_partition_erase_range((const esp_partition_t *)ffat_partition, dst_offset, bufsize);

  if(status != ESP_OK)
  {
    Serial.printf("esp_partition_erase_range failed status = %d\n", status);
  }

  status = esp_partition_write((const esp_partition_t *)ffat_partition, dst_offset, buffer, bufsize);

  if(status != ESP_OK)
  {
    Serial.printf("esp_partition_write failed status = %d\n", status);
  }

  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize){
  //Serial.printf("MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);

  size_t src_offset = (lba * FFAT_SECTOR_SIZE) + offset;
  esp_err_t status = esp_partition_read((const esp_partition_t *)ffat_partition, src_offset, buffer, bufsize);

  if(status != ESP_OK)
  {
    Serial.printf("esp_partition_read failed status = %d\n", status);
  }

  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  Serial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
  return true;
}

void init_msc()
{
  Serial.println("Configuring USB Mass Storage services...");

  ffat_partition = (esp_partition_t *)esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);

  Serial.printf("FFAT Flash Partition Name: %s Size: %d\n", ffat_partition->label, ffat_partition->size);

  USB.onEvent(usbEventCallback);

  MSC_Firmware.onEvent(usbEventCallback);
  MSC_Firmware.begin();
  
  MSC_Config.vendorID("PlacebCo");//max 8 chars
  MSC_Config.productID("AkbarFirmware");//max 16 chars
  MSC_Config.productRevision("0.2");//max 4 chars
  MSC_Config.onStartStop(onStartStop);
  MSC_Config.onRead(onRead);
  MSC_Config.onWrite(onWrite);
  MSC_Config.mediaPresent(true);
  MSC_Config.begin(ffat_partition->size / FFAT_SECTOR_SIZE, FFAT_SECTOR_SIZE);

  USB.begin();
}

void init_filesystem()
{
  //FFat.format(true);

  FFat.begin();

  Serial.printf("FFAT Free space: %10u\n", FFat.freeBytes());

  listDir(FFat, "/", 0);
}

void state_starting()
{
  init_pins();
  init_board_id();
  init_filesystem();
  init_config_data();
  init_msc();
  init_motor();
  init_tof_sensor();
  init_gnss();
  init_clock();
  calibrate();

  change_state(ACKBAR_STATE_ARMING);
}

void state_trapping()
{
  flip();
  delay(50);
  move_to_limit(HALL);
  delay(5000);
  flip();
  change_state(ACKBAR_STATE_REPORTING);
}

void state_reporting()
{
  send_notification("The trap was triggered!");
  change_state(ACKBAR_STATE_ARMING);
}

int consecutive_samples = 0;
void state_armed()
{

  flip();
  delay(1000);
  return;

  uint16_t distance_mm = tofDevice.readRangeSingleMillimeters();

  if(tofDevice.timeoutOccurred())
  {
    Serial.println("Timeout");
  }
  else
  {
    if(distance_mm < ANT_DETECTION_THRESHHOLD)
    {
      // Bugs interfering with sensor
      consecutive_samples = 0;
      delay(50);
    }
    else if(distance_mm < (calibrationData.tof_nominal_distance_mm - calibrationData.tof_trigger_distance_mm))
    {
      consecutive_samples++;

      if(consecutive_samples > 1)
      {
        Serial.print("Sensor distance: ");
        Serial.print(distance_mm);
        Serial.println(" mm");
        change_state(ACKBAR_STATE_TRAPPING);
      }
      else
      {
        delay(5);
      }
    }
    else
    {
      consecutive_samples = 0;
      delay(50);
    }
  }
}

void state_arming()
{
  uint16_t distance_mm = tofDevice.readRangeSingleMillimeters();

  if(tofDevice.timeoutOccurred())
  {
    Serial.println("Timeout");
  }
  else
  {
    if(distance_mm > (calibrationData.tof_nominal_distance_mm + calibrationData.tof_variance_mm * 1.5))
    {
      Serial.print("Sensor distance out of range: ");
      Serial.print(distance_mm);
      Serial.println(" mm");
      change_state(ACKBAR_STATE_ERROR);
    }
    else if(distance_mm > (calibrationData.tof_nominal_distance_mm - calibrationData.tof_variance_mm * 1.5))
    {
      Serial.print("Sensor distance within range: ");
      Serial.print(distance_mm);
      Serial.println(" mm");
      change_state(ACKBAR_STATE_ARMED);
    }
    else
    {
      Serial.print("Waiting for distance sensor to stabilize: ");
      Serial.print(distance_mm);
      Serial.println(" mm");
      delay(10);
    }
  }
}

void state_reset()
{
  change_state(ACKBAR_STATE_STARTING);
}

void state_error()
{

}

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  for(int i = 0; i < ACKBAR_STATE_LAST; i++)
  {
    if(dispatchTable[i].state == currentState)
    {
      (*dispatchTable[i].func)();
      break;
    }
  }
}
