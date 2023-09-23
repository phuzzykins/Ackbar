#include "Ackbar.h"
#include "AckbarDisplay.h"
#include "AckbarNotifier.h"
#include "AckbarVl53l0xTrigger.h"
#include "AckbarBarrelMechanism.h"

Ackbar                  trap;
AckbarDisplay           display;
AckbarNotifier          notifier;
AckbarVl53l0xTrigger    trigger;
AckbarBarrelMechanism   mechanism;

void setup()
{
  Serial.begin(115200);

  trap.addMechanism(mechanism);
  trap.addTrigger(trigger);
  trap.addEventConsumer(display);
  trap.addEventConsumer(notifier);
}

void loop()
{
  trap.doWork();
}
