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

void AckbarStatusLED::handleEvent(AckbarEvent * e)
{
  if(e->eventType = e->STATE_CHANGE_EVENT)
  {
    AckbarStateChangeEvent * sce = (AckbarStateChangeEvent *) e;
    switch(sce->newState)
    {
      case STATE_RESET:
        break;
      case STATE_ERROR:
        break;
      case STATE_STARTUP:
        break;
      case STATE_CALIBRATING:
        break;
      case STATE_ARMING:
        break;
      case STATE_ARMED:
        break;
      case STATE_ACTIVE:
        break;
      default:
        break;
    };

    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
  }
}