#include "Ackbar.h"

Ackbar::Ackbar()
{

}

void Ackbar::addTrigger(AckbarTrigger * t)
{
  trigger = t;
}

void Ackbar::addMechanism(AckbarMechanism * m)
{
  mechanism = m;
}

void Ackbar::addEventConsumer(AckbarEventConsumer & c)
{
  eventConsumers.push_back(c);
}

void Ackbar::doWork(void)
{
  
}