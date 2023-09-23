#ifndef __ACKBAR_VL53L0X_TRIGGER_H__
#define __ACKBAR_VL53L0X_TRIGGER_H__

#include "AckbarTrigger.h"
#include <VL53L0X.h>

class AckbarVl53l0xTrigger : public AckbarTrigger
{
  public:
    void begin();
    void calibrate();
    bool isReady();
    bool isTriggered();

  private:
    VL53L0X   device;
    uint16_t  trigger_distance_mm;
    uint16_t  nominal_distance_mm;
    uint16_t  variance_mm;
};


#endif // __ACKBAR_VL53L0X_TRIGGER_H__
