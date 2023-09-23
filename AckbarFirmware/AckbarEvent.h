#ifndef __ACKBAR_EVENT_H__
#define __ACKBAR_EVENT_H__

class AckbarEvent
{
  
};

class AckbarStartupEvent : public AckbarEvent
{

};

class AckbarTrapEvent : public AckbarEvent
{

};

class AckbarErrorEvent : public AckbarEvent
{

};

class AckbarStateChangeEvent : public AckbarEvent
{
  public:
    AckbarStateChangeEvent(int o, int n)
    {
      oldState = o;
      newState = n;
    };

    int oldState;
    int newState;


};

#endif // __ACKBAR_EVENT_H__
