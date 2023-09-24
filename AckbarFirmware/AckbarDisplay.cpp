#include "AckbarDisplay.h"
#include "AckbarPins.h"

#include <Fonts/FreeSans18pt7b.h>


char * AckbarDisplay::name()
{
  return("ePaper Display");
}

AckbarDisplay::~AckbarDisplay()
{
  if(epdDevice != nullptr)
  {
    delete epdDevice;
  }
}

void AckbarDisplay::begin()
{
  epdDevice = new Adafruit_SSD1681(200, 200, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);
  epdDevice->begin();
}

void AckbarDisplay::calibrate()
{

}

void AckbarDisplay::splashScreen()
{
  epdDevice->powerUp();
  epdDevice->clearBuffer();

  epdDevice->setFont(&FreeSans18pt7b);
  epdDevice->setCursor(0, 18);
  epdDevice->setTextColor(EPD_BLACK);
  epdDevice->setTextWrap(true);
  epdDevice->setCursor(8, 30);
  epdDevice->print("Ackbar v0.2:");
  epdDevice->setCursor(8, 60);
  epdDevice->print("It's a trap!");
  epdDevice->setCursor(8, 90);
  epdDevice->print(configuration->board_name);

  epdDevice->display();

  epdDevice->powerDown();
}

void AckbarDisplay::handleEvent(AckbarEvent * e)
{
  if(e->eventType == e->STARTUP_EVENT)
  {
    splashScreen();
  }
}