#include "Ackbar.h"
#include "AckbarEventService.h"

#include <FS.h>
#include <FFat.h>
#include <USB.h>

#define FFAT_SECTOR_SIZE 4096

static void readFile(fs::FS &fs, const char * path){
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

static void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
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
  }
  else if(event_base == ARDUINO_FIRMWARE_MSC_EVENTS)
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

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
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

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
  //Serial.printf("MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);

  size_t src_offset = (lba * FFAT_SECTOR_SIZE) + offset;
  esp_err_t status = esp_partition_read((const esp_partition_t *)ffat_partition, src_offset, buffer, bufsize);

  if(status != ESP_OK)
  {
    Serial.printf("esp_partition_read failed status = %d\n", status);
  }

  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject)
{
  Serial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
  return true;
}

Ackbar::Ackbar()
{
  strncpy(stateToString[STATE_RESET],       "RESET",        sizeof(stateToString[STATE_RESET]));
  strncpy(stateToString[STATE_ERROR],       "ERROR",        sizeof(stateToString[STATE_ERROR]));
  strncpy(stateToString[STATE_STARTUP],     "STARTUP",      sizeof(stateToString[STATE_STARTUP]));
  strncpy(stateToString[STATE_CALIBRATING], "CALIBRATING",  sizeof(stateToString[STATE_CALIBRATING]));
  strncpy(stateToString[STATE_ARMING],      "ARMING",       sizeof(stateToString[STATE_ARMING]));
  strncpy(stateToString[STATE_ARMED],       "ARMED",        sizeof(stateToString[STATE_ARMED]));
  strncpy(stateToString[STATE_ACTIVE],      "ACTIVE",       sizeof(stateToString[STATE_ACTIVE]));
}

void Ackbar::readConfiguration()
{
  FFat.begin();

  if(FFat.freeBytes() == 0)
  {
    Serial.println("Formatting FFAT");
    FFat.end();
    FFat.format(true);
    FFat.begin();
    Serial.println("Formatted FFAT");
  }

  Serial.printf("FFAT Free space: %10u\n", FFat.freeBytes());

  listDir(FFat, "/", 0);

  readFile(FFat, "/config.json");

  File config = FFat.open("/config.json", "r", false);

  if(! config)
  {
    Serial.println("Failed to open config.json");
    return;
  }

  configuration.fromStream(config);

  FFat.end();
}

void Ackbar::startMSC()
{
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

void Ackbar::addTrigger(AckbarTrigger * t)
{
  t->setConfiguration(&configuration);
  components.push_back(t);
  triggers.push_back(t);
}

void Ackbar::addMechanism(AckbarMechanism * m)
{
  m->setConfiguration(&configuration);
  components.push_back(m);
  mechanisms.push_back(m);
}

void Ackbar::addEventConsumer(AckbarEventConsumer * c)
{
  c->setConfiguration(&configuration);
  components.push_back(c);
  AckbarEventService s;
  s.addConsumer(c);
}

void Ackbar::changeState(AckbarState s)
{
  if(currentState == s)
  {
    return;
  }

  Serial.printf("Changing state from %s to %s\n", stateToString[currentState], stateToString[s]);

  AckbarStateChangeEvent * e = new AckbarStateChangeEvent(currentState, s);
  AckbarEventService es;

  es.publishEvent(e);

  currentState = s;
}

void Ackbar::doWork(void)
{
  switch(currentState)
  {
    case STATE_RESET:
      changeState(STATE_STARTUP);
      break;

    case STATE_STARTUP:
      for(AckbarComponent * c : components)
      {
        Serial.printf("Starting Component: %s\n", c->name());
        c->begin();
      }
      changeState(STATE_CALIBRATING);
      break;

    case STATE_CALIBRATING:
      for(AckbarComponent * c : components)
      {
        Serial.printf("Calibrating Component: %s\n", c->name());
        c->calibrate();
      }
      changeState(STATE_ARMING);
      break;

    case STATE_ARMING:
      {
        bool ready = true;
        for(AckbarTrigger * t : triggers)
        {

          if(t->isReady())
          {
            Serial.printf("Trigger: %s is READY to arm\n", t->name());
          }
          else
          {
            Serial.printf("Trigger: %s is NOT ready to arm\n", t->name());
            ready = false;
          }
        }

        if(ready)
        {
          changeState(STATE_ARMED);
        }
      };
      break;
    case STATE_ARMED:
      for(AckbarTrigger * t : triggers)
      {
        if(t->isTriggered())
        {
          Serial.printf("Activated by Trigger: %s\n", t->name());
          changeState(STATE_ACTIVE);
        }
      }
      break;

    case STATE_ACTIVE:
      {
        for(AckbarMechanism * m : mechanisms)
        {
          Serial.printf("Activating Mechanism: %s\n", m->name());
          m->activate();
        }

        delay(configuration.trap_dwell_time_ms);

        for(AckbarMechanism * m : mechanisms)
        {
          Serial.printf("Resetting Mechanism: %s\n", m->name());
          m->reset();
        }

        AckbarTrapEvent * e = new AckbarTrapEvent();
        AckbarEventService s;
        s.publishEvent(e);

        changeState(STATE_ARMING);
      };
      break;

    case STATE_ERROR:
      delay(100);
      break;

    default:
      changeState(STATE_ERROR);
      break;
  };
}