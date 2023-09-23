#ifndef __ACKBAR_EVENT_CONSUMER_H__
#define __ACKBAR_EVENT_CONSUMER_H__

#include "AckbarComponent.h"
#include "AckbarEvent.h"

class AckbarEventConsumer : public AckbarComponent
{
  public:
    void begin()                        {};
    void calibrate()                    {};
    void handleEvent(AckbarEvent & e)   {};
};


#endif // __ACKBAR_EVENT_CONSUMER_H__
