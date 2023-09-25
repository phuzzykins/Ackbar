#ifndef __ACKBAR_DISPLAY_H__
#define __ACKBAR_DISPLAY_H__

#include "AckbarEventConsumer.h"
#include <GxEPD2_BW.h>

class AckbarDisplay : public AckbarEventConsumer
{
  public:
    ~AckbarDisplay();
    char * name();
    void begin();
    void calibrate();
    void handleEvent(AckbarEvent * e);

  private:
    void splashScreen();
    
    GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> *epdDevice = nullptr;
};


#endif // __ACKBAR_DISPLAY_H__
