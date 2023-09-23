#include "AckbarStatusLED.h"

#include "AckbarEvent.h"
#include "AckbarPins.h"
#include "AckbarState.h"

void AckbarStatusLED::begin()
{
  pinMode(RGB_BUILTIN, OUTPUT);
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
}

void AckbarStatusLED::calibrate()
{

}

void AckbarStatusLED::handleEvent(AckbarEvent & e)
{
  if(e.eventType = e.STATE_CHANGE_EVENT)
  {
    AckbarStateChangeEvent * sce = (AckbarStateChangeEvent *) &e;
    switch(sce->newState)
    {
      case RESET:
        break;
      case ERROR:
        break;
      case STARTUP:
        break;
      case CALIBRATING:
        break;
      case ARMING:
        break;
      case ARMED:
        break;
      case ACTIVE:
        break;
      default:
        break;
    };

    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
  }
}