#ifndef __ACKBAR_H__
#define __ACKBAR_H__

#include <Arduino.h>

#include "AckbarConfiguration.h"
#include "AckbarEvent.h"
#include "AckbarEventConsumer.h"
#include "AckbarTrigger.h"
#include "AckbarMechanism.h"
#include "AckbarState.h"

#include <list>
#include <queue>
#include <mutex>
#include <thread>

class Ackbar
{
  public:
    Ackbar();

    void readConfiguration();
    void startMSC();

    void addTrigger(AckbarTrigger & t);
    void addMechanism(AckbarMechanism & m);
    void addEventConsumer(AckbarEventConsumer & c);
    void publishEvent(AckbarEvent & e);

    void doWork(void);

  private:
    AckbarConfiguration              configuration;

    std::list<AckbarTrigger>         triggers;
    std::list<AckbarMechanism>       mechanisms;
    std::list<AckbarEventConsumer>   eventConsumers;

    std::queue<AckbarEvent>          eventQueue;
    std::mutex                       eventQueueLock;
    std::thread *                    pEventThread       = nullptr;
    
    AckbarState                      currentState       = RESET;

    void changeState(AckbarState s);
};


#endif // __ACKBAR_H__