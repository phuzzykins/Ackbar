#ifndef __ACKBAR_H__
#define __ACKBAR_H__

#include "AckbarPins.h"

#include <BasicStepperDriver.h>
#include <VL53L0X.h>
#include <TMC2209.h>
#include <SparkFun_I2C_GPS_Arduino_Library.h>
#include <Adafruit_EPD.h>
#include <USBMSC.h>
#include <FirmwareMSC.h>

class Ackbar
{
  public:
    void doWork(void);

  protected:
    TMC2209             *stepperDriver = NULL;
    BasicStepperDriver  *stepperMotor = NULL;
    I2CGPS              *gnssDevice = NULL;
    VL53L0X             *tofDevice = NULL;
    Adafruit_SSD1608    *epdDevice = NULL;
    FirmwareMSC         *MSC_Firmware = NULL;
    USBMSC              *MSC_Config = NULL;
};


#endif // __ACKBAR_H__