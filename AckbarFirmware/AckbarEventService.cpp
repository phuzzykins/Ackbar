#include "AckbarEventService.h"


std::mutex                       AckbarEventService::lock;
std::list<AckbarEventConsumer*>  AckbarEventService::eventConsumers   = std::list<AckbarEventConsumer*>();
std::queue<AckbarEvent *>        AckbarEventService::eventQueue       = std::queue<AckbarEvent *>();
std::thread *                    AckbarEventService::pEventThread     = nullptr;

void AckbarEventService::addConsumer(AckbarEventConsumer * c)
{
  eventConsumers.push_back(c);
}

void AckbarEventService::publishEvent(AckbarEvent * e)
{
  std::lock_guard<std::mutex> guard(AckbarEventService::lock);

  //eventQueue.push(e);

  for(AckbarEventConsumer * c : AckbarEventService::eventConsumers)
  {
    //Serial.printf("Sending Event of type %d to: %s\n", e->eventType, c->name());
    c->handleEvent(e);
  }

  delete(e);
}
