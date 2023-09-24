#ifndef __ACKBAR_EVENT_H__
#define __ACKBAR_EVENT_H__

class AckbarEvent
{
  public:
    int eventType = -1;
    enum EventType
    {
      TRAP_EVENT,
      ERROR_EVENT,
      STATE_CHANGE_EVENT
    };
};

class AckbarTrapEvent : public AckbarEvent
{
  public:
    AckbarTrapEvent(void)
    {
      Serial.println("Constructor for AckbarTrapEvent");
      eventType = TRAP_EVENT;
    };
};

class AckbarErrorEvent : public AckbarEvent
{
  public:
    AckbarErrorEvent()
    {
      eventType = ERROR_EVENT;
    };
};

class AckbarStateChangeEvent : public AckbarEvent
{
  public:
    AckbarStateChangeEvent(int o, int n)
    {
      eventType = STATE_CHANGE_EVENT;
      oldState = o;
      newState = n;
    };

    int oldState;
    int newState;
};

#endif // __ACKBAR_EVENT_H__
