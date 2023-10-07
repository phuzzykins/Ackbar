#ifndef __ACKBAR_DISPLAY_H__
#define __ACKBAR_DISPLAY_H__

#include "AckbarEventConsumer.h"
#include <qrcodedisplay.h>
#include <GxEPD2_BW.h>

class AckbarDisplay : public AckbarEventConsumer, public QRcodeDisplay
{
  public:
    ~AckbarDisplay();
    char * name();
    bool begin();
    bool calibrate();
    void handleEvent(AckbarEvent * e);
		void screenwhite();
		void screenupdate();

  private:
    void splashScreen();
    void drawPixel(int x, int y, int color);
    GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> *epdDevice = nullptr;
};


#endif // __ACKBAR_DISPLAY_H__
