#ifndef __ACKBAR_BARREL_MECHANISM_H__
#define __ACKBAR_BARREL_MECHANISM_H__

#include "AckbarMechanism.h"

#include <TMC2209.h>
#include <BasicStepperDriver.h>

class AckbarBarrelMechanism : public AckbarMechanism
{
  public:
    ~AckbarBarrelMechanism();
    void begin();
    void calibrate();
    void activate();
    void reset();

  private:
    TMC2209             * stepperDriver = nullptr;
    BasicStepperDriver  * stepperMotor  = nullptr;

    bool limitSwitchTriggered();
    bool seekLimit();
    void flip();
};


#endif // __ACKBAR_BARREL_MECHANISM_H__
