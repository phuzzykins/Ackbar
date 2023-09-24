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

void Ackbar::addTrigger(AckbarTrigger * t)
{
  t->setConfiguration(&configuration);
  triggers.push_back(t);
}

void Ackbar::addMechanism(AckbarMechanism * m)
{
  m->setConfiguration(&configuration);
  mechanisms.push_back(m);
}

void Ackbar::addEventConsumer(AckbarEventConsumer * c)
{
  c->setConfiguration(&configuration);
  eventConsumers.push_back(c);
}

void Ackbar::publishEvent(AckbarEvent * e)
{
  //std::lock_guard<std::mutex> guard(eventQueueLock);

  //eventQueue.push(e);

  for(AckbarEventConsumer * c : eventConsumers)
  {
    c->handleEvent(e);
  }

  delete(e);
}

void Ackbar::changeState(AckbarState s)
{
  if(currentState == s)
  {
    return;
  }

  Serial.printf("Changing state from %d to %d\n", currentState, s);

  AckbarStateChangeEvent * e = new AckbarStateChangeEvent(currentState, s);

  publishEvent(e);

  currentState = s;
}

void Ackbar::doWork(void)
{
  switch(currentState)
  {
    case STATE_RESET:
      changeState(STATE_STARTUP);
      break;

    case STATE_STARTUP:
      for(AckbarMechanism * m : mechanisms)
      {
        m->begin();
      }
      for(AckbarTrigger * t : triggers)
      {
        t->begin();
      }
      for(AckbarEventConsumer * c : eventConsumers)
      {
        c->begin();
      }
      changeState(STATE_CALIBRATING);
      break;

    case STATE_CALIBRATING:
      for(AckbarEventConsumer * c : eventConsumers)
      {
        c->calibrate();
      }
      for(AckbarMechanism * m : mechanisms)
      {
        m->calibrate();
      }
      for(AckbarTrigger * t : triggers)
      {
        t->calibrate();
      }
      changeState(STATE_ARMING);
      break;

    case STATE_ARMING:
      for(AckbarTrigger * t : triggers)
      {
        if(t->isReady())
        {
          changeState(STATE_ARMING);
        }
      }

    case STATE_ARMED:
      for(AckbarTrigger * t : triggers)
      {
        if(t->isTriggered())
        {
          changeState(STATE_ACTIVE);
        }
      }

    case STATE_ACTIVE:
      for(AckbarMechanism * m : mechanisms)
      {
        m->activate();
      }
      delay(configuration.trap_dwell_time_ms);
      for(AckbarMechanism * m : mechanisms)
      {
        m->reset();
      }
      //AckbarTrapEvent * e = new AckbarTrapEvent();
      //publishEvent(e);
      changeState(STATE_ARMING);
      break;

    case STATE_ERROR:
      delay(100);
      break;

    default:
      changeState(STATE_ERROR);
      break;
  };
}