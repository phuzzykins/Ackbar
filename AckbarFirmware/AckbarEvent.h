#ifndef __ACKBAR_EVENT_H__
#define __ACKBAR_EVENT_H__

#include <IPAddress.h>

class AckbarEvent
{
  public:
    int eventType = -1;
    enum EventType
    {
      STATE_CHANGE_EVENT,
      STARTUP_EVENT,
      TRAP_EVENT,
      ERROR_EVENT,
      WIFI_CONNECTION_EVENT,
      WIFI_DISCONNECTION_EVENT,
      LINK_EVENT
    };
};

class AckbarLinkEvent : public AckbarEvent
{
  public:
    AckbarLinkEvent(String uri)
    {
      this->eventType = LINK_EVENT;
      this->uri       = uri;
    };
    String uri;
};

class AckbarTrapEvent : public AckbarEvent
{
  public:
    AckbarTrapEvent(void)
    {
      eventType = TRAP_EVENT;
    };
};

class AckbarErrorEvent : public AckbarEvent
{
  public:
    AckbarErrorEvent(String msg)
    {
      this->eventType = ERROR_EVENT;
      this->msg       = msg;
    };
    String msg;
};

class AckbarStateChangeEvent : public AckbarEvent
{
  public:
    AckbarStateChangeEvent(int oldState, int newState)
    {
      eventType       = STATE_CHANGE_EVENT;
      this->oldState  = oldState;
      this->newState  = newState;
    };

    int oldState;
    int newState;
};

class AckbarStartupEvent : public AckbarEvent
{
  public:
    AckbarStartupEvent()
    {
      eventType = STARTUP_EVENT;
    };
};

class AckbarWifiConnectionEvent : public AckbarEvent
{
  public:
    AckbarWifiConnectionEvent(IPAddress ipAddress, int rssi)
    {
      eventType       = WIFI_CONNECTION_EVENT;
      this->ipAddress = ipAddress;
      this->rssi      = rssi;
    };

    ~AckbarWifiConnectionEvent()
    {
      //delete ipAddress;
    }

    IPAddress ipAddress;
    int rssi;
};

class AckbarWifiDisconnectionEvent : public AckbarEvent
{
  public:
    AckbarWifiDisconnectionEvent()
    {
      eventType = WIFI_DISCONNECTION_EVENT;
    };
};

#endif // __ACKBAR_EVENT_H__
