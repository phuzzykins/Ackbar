#ifndef __ACKBAR_H__
#define __ACKBAR_H__

#include "AckbarPins.h"
#include "AckbarEvent.h"
#include "AckbarEventConsumer.h"
#include "AckbarTrigger.h"
#include "AckbarMechanism.h"

#include <list>
#include <queue>

class Ackbar
{
  public:
    Ackbar();

    void addTrigger(AckbarTrigger * t);
    void addMechanism(AckbarMechanism * m);
    void addEventConsumer(AckbarEventConsumer & c);

    void doWork(void);

  private:
    AckbarTrigger *                  trigger        = nullptr;
    AckbarMechanism *                mechanism      = nullptr;

    std::list<AckbarEventConsumer>   eventConsumers = std::list<AckbarEventConsumer>();
    std::queue<AckbarEvent>          eventQueue     = std::queue<AckbarEvent>();
};


#endif // __ACKBAR_H__