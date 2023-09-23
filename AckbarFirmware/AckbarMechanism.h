#ifndef __ACKBAR_MECHANISM_H__
#define __ACKBAR_MECHANISM_H__

#include "AckbarComponent.h"

class AckbarMechanism : public AckbarComponent
{
  public:
    void begin()      {};
    void calibrate()  {};

    void activate()   {};
    void reset()      {};
};


#endif // __ACKBAR_Mechanism_H__
