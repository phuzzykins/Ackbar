#include "AckbarStatusLED.h"

#include "AckbarEvent.h"
#include "AckbarPins.h"
#include "AckbarState.h"

AckbarStatusLED::AckbarStatusLED()
{
  deliverEventsSynchronously = true;
  
  pixels = new Adafruit_NeoPixel(1, RGB_BUILTIN, NEO_RGB + NEO_KHZ800);

  stateToColor[STATE_RESET]       = pixels->Color(255, 255, 255);
  stateToColor[STATE_ERROR]       = pixels->Color(255, 0, 0);
  stateToColor[STATE_STARTUP]     = pixels->Color(64, 0, 64);
  stateToColor[STATE_CALIBRATING] = pixels->Color(0, 0, 64);
  stateToColor[STATE_ARMING]      = pixels->Color(64, 64, 0);
  stateToColor[STATE_ARMED]       = pixels->Color(0, 64, 0);
  stateToColor[STATE_ACTIVE]      = pixels->Color(64, 64, 64);

}

AckbarStatusLED::~AckbarStatusLED()
{
  if(pixels != nullptr)
  {
    delete pixels;
  }
}

char * AckbarStatusLED::name()
{
  return("RGB Status LED");
}


bool AckbarStatusLED::begin()
{

  return true;
}

bool AckbarStatusLED::calibrate()
{
  return true;
}

void AckbarStatusLED::handleEvent(AckbarEvent * e)
{
  if(pixels == nullptr)
  {
    return;
  }

  if(e->eventType == e->STATE_CHANGE_EVENT)
  {
    AckbarStateChangeEvent * sce = (AckbarStateChangeEvent *) e;

    pixels->setPixelColor(0, stateToColor[sce->newState]);
    pixels->show();
  }
}