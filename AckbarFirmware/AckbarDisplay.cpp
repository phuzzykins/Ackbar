#include "AckbarDisplay.h"
#include "AckbarPins.h"

#include "qrencode.h"
#include <Fonts/FreeSans12pt7b.h>


char * AckbarDisplay::name()
{
  return("ePaper Display");
}

AckbarDisplay::~AckbarDisplay()
{
  delete epdDevice;
}


bool AckbarDisplay::begin()
{
  epdDevice = new GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>(GxEPD2_154_D67(EPD_CS, EPD_DC, EPD_RESET, EPD_BUSY));

  epdDevice->init();
  epdDevice->setRotation(3);

  screenwidth   = epdDevice->width();
  screenheight  = epdDevice->height();

  epdDevice->setFullWindow();
  epdDevice->fillScreen(GxEPD_WHITE);

  int min_dimension = min(screenwidth, screenheight);

  multiply = min_dimension/WD;
  offsetsX = (screenwidth-(WD*multiply))/2;
  offsetsY = (screenheight-(WD*multiply))/2;

  return true;
}

bool AckbarDisplay::calibrate()
{
  return true;
}

void AckbarDisplay::errorScreen(String msg)
{
  epdDevice->setFullWindow();
  epdDevice->fillScreen(GxEPD_WHITE);
  epdDevice->setFont(&FreeSans12pt7b);
  epdDevice->setTextColor(GxEPD_BLACK);
  epdDevice->setTextWrap(true);
  epdDevice->setCursor(0, 18);
  epdDevice->print(msg);


  epdDevice->display(false);
}

void AckbarDisplay::handleEvent(AckbarEvent * e)
{
  if(epdDevice == nullptr)
  {
    // Not ready to handle events yet
    return;
  }

  if(e->eventType == e->ERROR_EVENT)
  {
    AckbarErrorEvent * err = (AckbarErrorEvent *)e;
    Serial.print("Display received ERROR event: ");
    Serial.println(err->msg);
    errorScreen(err->msg);
  }
  else if(e->eventType == e->LINK_EVENT)
  {
    AckbarLinkEvent * link = (AckbarLinkEvent *)e;

    Serial.print("Display received Link Event: ");
    Serial.println(link->uri);

    this->QRcodeDisplay::create(link->uri);
  }
}


void AckbarDisplay::screenwhite() {
    epdDevice->fillScreen(GxEPD_WHITE);
}

void AckbarDisplay::screenupdate() {
    epdDevice->display();
}

void AckbarDisplay::drawPixel(int x, int y, int color) {
    if(color==1) {
        color = GxEPD_BLACK;
    } else {
        color = GxEPD_WHITE;
    }
    epdDevice->fillRect(x,y,multiply,multiply,color);
}