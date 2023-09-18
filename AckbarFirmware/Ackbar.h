#ifndef __ACKBAR_H__
#define __ACKBAR_H__

#include <Arduino.h>
#include <FS.h>
#include <FFat.h>
#include <USB.h>
#include <USBMSC.h>
#include <FirmwareMSC.h>
#include <TMC2209.h>
#include <BasicStepperDriver.h>
#include <VL53L0X.h>

class Ackbar : public
{
  public:
    void doWork(void);

  protected:
	TMC2209             stepperDriver;
	BasicStepperDriver  stepperMotor;
	I2CGPS              gnssDevice;
	VL53L0X             tofDevice;
	Adafruit_SSD1608    epdDevice;
	FirmwareMSC         MSC_Firmware;
	USBMSC              MSC_Config;
};


#endif // __ACKBAR_H__