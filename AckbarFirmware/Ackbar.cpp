#include "Ackbar.h"

Ackbar::Ackbar()
{

}

void Ackbar::addTrigger(AckbarTrigger & t)
{
  triggers.push_back(t);
}

void Ackbar::addMechanism(AckbarMechanism & m)
{
  mechanisms.push_back(m);
}

void Ackbar::addEventConsumer(AckbarEventConsumer & c)
{
  eventConsumers.push_back(c);
}

void Ackbar::publishEvent(AckbarEvent & e)
{
  std::lock_guard<std::mutex> guard(eventQueueLock);
  
  eventQueue.push(e);
}

void Ackbar::doWork(void)
{
  
}