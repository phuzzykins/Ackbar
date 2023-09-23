#ifndef __ACKBAR_TRIGGER_H__
#define __ACKBAR_TRIGGER_H__

#include "AckbarComponent.h"

class AckbarTrigger : public AckbarComponent
{
  public:
    void begin()        {};
    void calibrate()    {};

    bool isReady()      { return true;  }
    bool isTriggered()  { return false; }
};


#endif // __ACKBAR_TRIGGER_H__
