#ifndef __ACKBAR_STATUS_LED_H__
#define __ACKBAR_STATUS_LED_H__

#include "AckbarEventConsumer.h"
#include "AckbarState.h"

#include <Adafruit_NeoPixel.h>

class AckbarStatusLED : public AckbarEventConsumer
{
  public:
    AckbarStatusLED();
    ~AckbarStatusLED();
    char * name();
    void begin();
    void calibrate();
    void handleEvent(AckbarEvent * e);

  private:
    uint32_t stateToColor[STATE_LAST];

    Adafruit_NeoPixel * pixels = nullptr;
};


#endif // __ACKBAR_STATUS_LED_H__
