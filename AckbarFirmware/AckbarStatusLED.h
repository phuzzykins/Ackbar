#ifndef __ACKBAR_STATUS_LED_H__
#define __ACKBAR_STATUS_LED_H__

#include "AckbarEventConsumer.h"

class AckbarStatusLED : public AckbarEventConsumer
{
  public:
    void begin();
    void calibrate();
    void handleEvent(AckbarEvent * e);
};


#endif // __ACKBAR_STATUS_LED_H__
