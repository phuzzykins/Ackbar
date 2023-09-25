#include "AckbarDisplay.h"
#include "AckbarPins.h"

#include "qrencode.h"
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

  this->screenwidth   = epdDevice->width();
  this->screenheight  = epdDevice->height();

  epdDevice->setFullWindow();
  epdDevice->fillScreen(GxEPD_WHITE);

  int min = screenwidth;
  if(screenheight < screenwidth)
  {
    min = screenheight;
  }
        
  multiply = min/WD;
  offsetsX = (screenwidth-(WD*multiply))/2;
  offsetsY = (screenheight-(WD*multiply))/2;
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
  if(epdDevice == nullptr)
  {
    // Not ready to handle events yet
    return;
  }

  if(e->eventType == e->STARTUP_EVENT)
  {
    Serial.println("Display received startup event");
    //splashScreen();
  }
  else if(e->eventType == e->LINK_EVENT)
  {
    AckbarLinkEvent * link = (AckbarLinkEvent *)e;

    Serial.print("Display received Link Event: ");
    Serial.println(link->uri);

    create(link->uri);
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