#ifndef __ACKBAR_H__
#define __ACKBAR_H__

#include <Arduino.h>

#include "AckbarConfiguration.h"
#include "AckbarEventConsumer.h"
#include "AckbarTrigger.h"
#include "AckbarMechanism.h"
#include "AckbarState.h"

#include <list>
#include <queue>
#include <mutex>
#include <thread>

#include <USBMSC.h>
#include <FirmwareMSC.h>

class Ackbar
{
  public:
    Ackbar();

    void readConfiguration();
    void startMSC();

    void addTrigger(AckbarTrigger * t);
    void addMechanism(AckbarMechanism * m);
    void addEventConsumer(AckbarEventConsumer * c);

    void doWork(void);

  private:
    AckbarConfiguration              configuration;

    FirmwareMSC                      MSC_Firmware;
    USBMSC                           MSC_Config;

    std::list<AckbarComponent*>      components;
    std::list<AckbarTrigger*>        triggers;
    std::list<AckbarMechanism*>      mechanisms;
    
    AckbarState                      currentState       = STATE_RESET;

    char stateToString[STATE_LAST][32];

    void changeState(AckbarState s);
};


#endif // __ACKBAR_H__