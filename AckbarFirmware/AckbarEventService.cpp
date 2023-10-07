#include "AckbarEventService.h"

std::mutex                       AckbarEventService::lock;
std::list<AckbarEventConsumer*>  AckbarEventService::eventConsumers   = std::list<AckbarEventConsumer*>();
std::queue<AckbarEvent *>        AckbarEventService::eventQueue       = std::queue<AckbarEvent *>();
TaskHandle_t                     AckbarEventService::pTaskHandle      = nullptr;

static void eventTaskFunction(void * pvParameters)
{
  AckbarEventService s;

  s.eventThreadFunction();
}

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
      delay(100);
    }
    else
    {
      for(AckbarEventConsumer * c : AckbarEventService::eventConsumers)
      {
        if(! c->deliverEventsSynchronously) {
          //Serial.printf("Sending Event of type %d to: %s\n", e->eventType, c->name());
          c->handleEvent(e);
        }
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
  for(AckbarEventConsumer * c : AckbarEventService::eventConsumers)
  {
    if(c->deliverEventsSynchronously) {
      //Serial.printf("Sending Event of type %d to: %s\n", e->eventType, c->name());
      c->handleEvent(e);
    }
  }

  std::lock_guard<std::mutex> guard(AckbarEventService::lock);

  eventQueue.push(e);

  if(AckbarEventService::pTaskHandle == nullptr)
  {
    xTaskCreatePinnedToCore(
      &eventTaskFunction,
      "Event Dispatch Thread",
      8192,
      nullptr,
      1,
      &AckbarEventService::pTaskHandle,
      (ARDUINO_RUNNING_CORE == 0) ? 1 : 0
    );
  }
}
