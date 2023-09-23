#ifndef __ACKBAR_VL53L0X_TRIGGER_H__
#define __ACKBAR_VL53L0X_TRIGGER_H__

#include "AckbarTrigger.h"

class AckbarVl53l0xTrigger : public AckbarTrigger
{
  public:
    void begin();
    void calibrate();
};


#endif // __ACKBAR_VL53L0X_TRIGGER_H__
