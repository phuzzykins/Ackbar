#ifndef __ACKBAR_NOTIFIER_H__
#define __ACKBAR_NOTIFIER_H__

#include "AckbarEventConsumer.h"

class AckbarNotifier : public AckbarEventConsumer
{
  public:
    void begin();
    void calibrate();
};


#endif // __ACKBAR_NOTIFIER_H__
