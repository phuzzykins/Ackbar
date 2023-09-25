#include "AckbarDisplay.h"
#include "AckbarPins.h"

#include <Fonts/FreeSans18pt7b.h>


char * AckbarDisplay::name()
{
  return("ePaper Display");
}

AckbarDisplay::~AckbarDisplay()
{
  delete epdDevice;
}

void AckbarDisplay::begin()
{
  epdDevice = new GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>(GxEPD2_154_D67(EPD_CS, EPD_DC, EPD_RESET, EPD_BUSY));

  epdDevice->init(115200);
  epdDevice->setRotation(3);
}

void AckbarDisplay::calibrate()
{

}

void AckbarDisplay::splashScreen()
{
  epdDevice->setFullWindow();
  epdDevice->fillScreen(GxEPD_WHITE);
  epdDevice->setFont(&FreeSans18pt7b);
  epdDevice->setCursor(0, 18);
  epdDevice->setTextColor(GxEPD_BLACK);
  epdDevice->setTextWrap(true);
  epdDevice->setCursor(8, 30);
  epdDevice->print("Ackbar v0.2:");
  epdDevice->setCursor(8, 60);
  epdDevice->print("It's a trap!");
  epdDevice->setCursor(8, 90);
  epdDevice->print(configuration->board_name);

  epdDevice->display(false);
}

void AckbarDisplay::handleEvent(AckbarEvent * e)
{
  if(e->eventType == e->STARTUP_EVENT)
  {
    splashScreen();
  }
}