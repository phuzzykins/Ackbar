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

  private:
    void eventThreadFunction();
    static std::mutex                       lock;
    static std::list<AckbarEventConsumer*>  eventConsumers;
    static std::queue<AckbarEvent *>        eventQueue;
    static std::thread *                    pEventThread;
};

#endif // __ACKBAR_EVENT_SERVICE_H__