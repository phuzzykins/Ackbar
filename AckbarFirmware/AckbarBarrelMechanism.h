#ifndef __ACKBAR_BARREL_MECHANISM_H__
#define __ACKBAR_BARREL_MECHANISM_H__

#include "AckbarMechanism.h"

#include <TMC2209.h>
#include <BasicStepperDriver.h>

class AckbarBarrelMechanism : public AckbarMechanism
{
  public:
    ~AckbarBarrelMechanism();
    char * name();
    bool begin();
    bool calibrate();
    void activate();
    void reset();

  private:
    TMC2209             * stepperDriver = nullptr;
    BasicStepperDriver  * stepperMotor  = nullptr;

    void getDriverSettings();
    bool limitSwitchTriggered();
    bool seekLimit();
    void flip();
};


#endif // __ACKBAR_BARREL_MECHANISM_H__
