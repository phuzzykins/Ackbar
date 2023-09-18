#include <BasicStepperDriver.h>
#include <VL53L0X.h>
#include <TMC2209.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <SparkFun_I2C_GPS_Arduino_Library.h>
#include <Adafruit_EPD.h>

#define STP_MS1   17
#define STP_MS2   18
#define STP_MS3   33
#define STP_PDN   34
#define STP_DIAG  35
#define STP_EN    38
#define STP_STEP  39
#define STP_DIR   40

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

TMC2209             stepperDriver;
BasicStepperDriver  stepperMotor(STEPPER_STEPS_PER_REV, STP_DIR, STP_STEP, STP_EN);
I2CGPS              gnssDevice;
VL53L0X             tofDevice;

Adafruit_SSD1608 epdDevice(200, 200, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);


void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
