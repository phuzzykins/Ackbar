#ifndef __ACKBAR_NOTIFIER_H__
#define __ACKBAR_NOTIFIER_H__

#include "AckbarEventConsumer.h"

class AckbarNotifier : public AckbarEventConsumer
{
  public:
    char * name();
    void begin();
    void calibrate();
    void handleEvent(AckbarEvent * e);

  private:
   String uri;
   int missedTrapNotifications = 0;
};


#endif // __ACKBAR_NOTIFIER_H__
