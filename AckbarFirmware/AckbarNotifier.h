#ifndef __ACKBAR_NOTIFIER_H__
#define __ACKBAR_NOTIFIER_H__

#include "AckbarEventConsumer.h"

#define BOARD_ID_LENGTH         16

class AckbarNotifier : public AckbarEventConsumer
{
  public:
    char * name();
    void begin();
    void calibrate();
    void handleEvent(AckbarEvent * e);

  private:
   char board_id[BOARD_ID_LENGTH];
};


#endif // __ACKBAR_NOTIFIER_H__
