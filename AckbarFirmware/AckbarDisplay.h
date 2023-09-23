#ifndef __ACKBAR_DISPLAY_H__
#define __ACKBAR_DISPLAY_H__

#include "AckbarEventConsumer.h"

class AckbarDisplay : public AckbarEventConsumer
{
  public:
    void begin();
    void calibrate();
};


#endif // __ACKBAR_DISPLAY_H__
