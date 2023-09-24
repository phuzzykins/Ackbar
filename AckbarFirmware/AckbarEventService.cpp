#include "AckbarEventService.h"


std::mutex                       AckbarEventService::lock;
std::list<AckbarEventConsumer*>  AckbarEventService::eventConsumers   = std::list<AckbarEventConsumer*>();
std::queue<AckbarEvent *>        AckbarEventService::eventQueue       = std::queue<AckbarEvent *>();
std::thread *                    AckbarEventService::pEventThread     = nullptr;

void AckbarEventService::eventThreadFunction()
{
  while(true)
  {
    AckbarEvent * e = nullptr;
    {
      std::lock_guard<std::mutex> guard(AckbarEventService::lock);
      if(AckbarEventService::eventQueue.size() > 0)
      {
        e = AckbarEventService::eventQueue.front();
        AckbarEventService::eventQueue.pop();
      }
    };

    if(e == nullptr)
    {
      delay(10);
    }
    else
    {
      std::lock_guard<std::mutex> guard(AckbarEventService::lock);
      for(AckbarEventConsumer * c : AckbarEventService::eventConsumers)
      {
        //Serial.printf("Sending Event of type %d to: %s\n", e->eventType, c->name());
        c->handleEvent(e);
      }

      delete(e);
    }
  }
}

void AckbarEventService::addConsumer(AckbarEventConsumer * c)
{
  std::lock_guard<std::mutex> guard(AckbarEventService::lock);
  eventConsumers.push_back(c);
}

void AckbarEventService::publishEvent(AckbarEvent * e)
{
  std::lock_guard<std::mutex> guard(AckbarEventService::lock);

  eventQueue.push(e);

  if(AckbarEventService::pEventThread == nullptr)
  {
    //AckbarEventService::pEventThread = new std::thread(&threadStart);
    AckbarEventService::pEventThread = new std::thread(&AckbarEventService::eventThreadFunction, this);
  }
}
