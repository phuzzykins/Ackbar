#ifndef __ACKBAR_EVENT_SERVICE_H__
#define __ACKBAR_EVENT_SERVICE_H__

#include "AckbarEvent.h"
#include "AckbarEventConsumer.h"

#include <list>
#include <queue>
#include <mutex>
#include <thread>

class AckbarEventService
{
  public:
    void addConsumer(AckbarEventConsumer * c);
    void publishEvent(AckbarEvent * e);
    void eventThreadFunction();

  private:
    static std::mutex                       lock;
    static std::list<AckbarEventConsumer*>  eventConsumers;
    static std::queue<AckbarEvent *>        eventQueue;
    static TaskHandle_t                     pTaskHandle;
};

#endif // __ACKBAR_EVENT_SERVICE_H__