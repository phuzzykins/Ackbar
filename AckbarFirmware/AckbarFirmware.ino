#include "Ackbar.h"
#include "AckbarStatusLED.h"
#include "AckbarDisplay.h"
#include "AckbarNotifier.h"
#include "AckbarVl53l0xTrigger.h"
#include "AckbarBarrelMechanism.h"

Ackbar                  trap;
AckbarStatusLED         led;
AckbarDisplay           display;
AckbarNotifier          notifier;
AckbarVl53l0xTrigger    trigger;
AckbarBarrelMechanism   mechanism;

void setup()
{
  Serial.begin(115200);

  Serial.println("Ackbar firmware is starting up");

  trap.readConfiguration();
  trap.startMSC();

  trap.addMechanism(&mechanism);
  trap.addTrigger(&trigger);
  trap.addEventConsumer(&led);
  trap.addEventConsumer(&display);
  trap.addEventConsumer(&notifier);

  Serial.println("Ackbar firmware topology is configured");
}

void loop()
{
  trap.doWork();
}
