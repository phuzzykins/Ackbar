#include "Ackbar.h"

Ackbar::Ackbar()
{

}

void Ackbar::readConfiguration()
{

}

void Ackbar::startMSC()
{

}

void Ackbar::addTrigger(AckbarTrigger & t)
{
  t.setConfiguration(&configuration);
  triggers.push_back(t);
}

void Ackbar::addMechanism(AckbarMechanism & m)
{
  m.setConfiguration(&configuration);
  mechanisms.push_back(m);
}

void Ackbar::addEventConsumer(AckbarEventConsumer & c)
{
  c.setConfiguration(&configuration);
  eventConsumers.push_back(c);
}

void Ackbar::publishEvent(AckbarEvent & e)
{
  std::lock_guard<std::mutex> guard(eventQueueLock);

  eventQueue.push(e);
}

void Ackbar::changeState(AckbarState s)
{
  if(currentState == s)
  {
    return;
  }

  AckbarStateChangeEvent e(currentState, s);

  currentState = s;

  publishEvent(e);
}

void Ackbar::doWork(void)
{
  switch(currentState)
  {
    case RESET:
      changeState(STARTUP);
      break;
    case STARTUP:
      break;
    case CALIBRATING:
      break;
    case ARMING:
      break;
    case ARMED:
      break;
    case ACTIVE:
      break;
    case ERROR:
      delay(100);
      break;
    default:
      changeState(ERROR);
      break;
  };
}