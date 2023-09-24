#ifndef __ACKBAR_DISPLAY_H__
#define __ACKBAR_DISPLAY_H__

#include "AckbarEventConsumer.h"
#include <Adafruit_EPD.h>

class AckbarDisplay : public AckbarEventConsumer
{
  public:
    ~AckbarDisplay();
    void begin();
    void calibrate();
    void handleEvent(AckbarEvent * e);

  public:
    Adafruit_SSD1681 *epdDevice = nullptr;
};


#endif // __ACKBAR_DISPLAY_H__
