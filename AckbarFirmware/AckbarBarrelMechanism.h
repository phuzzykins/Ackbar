#ifndef __ACKBAR_BARREL_MECHANISM_H__
#define __ACKBAR_BARREL_MECHANISM_H__

#include "AckbarMechanism.h"

class AckbarBarrelMechanism : public AckbarMechanism
{
  public:
    void begin();
    void calibrate();
};


#endif // __ACKBAR_BARREL_MECHANISM_H__
