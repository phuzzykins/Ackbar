#ifndef __ACKBAR_EVENT_CONSUMER_H__
#define __ACKBAR_EVENT_CONSUMER_H__

#include "AckbarComponent.h"
#include "AckbarEvent.h"

class AckbarEventConsumer : public AckbarComponent
{
  public:
    virtual void handleEvent(AckbarEvent * e) = 0;
    bool deliverEventsSynchronously = false;
};


#endif // __ACKBAR_EVENT_CONSUMER_H__
