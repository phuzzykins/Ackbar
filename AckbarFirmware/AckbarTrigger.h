#ifndef __ACKBAR_TRIGGER_H__
#define __ACKBAR_TRIGGER_H__

#include "AckbarComponent.h"

class AckbarTrigger : public AckbarComponent
{
  public:
    virtual bool isReady() = 0;
    virtual bool isTriggered() = 0;
};


#endif // __ACKBAR_TRIGGER_H__
