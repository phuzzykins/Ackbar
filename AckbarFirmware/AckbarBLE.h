#ifndef __ACKBAR_BLE_H__
#define __ACKBAR_BLE_H__

#include "AckbarEventConsumer.h"

#include <BLEDevice.h>
#include <BLEServer.h>

#include <map>

#define ACKBAR_BLE_SERVICE_UUID "589a76d3-83b7-4d3c-a503-ad26c902c9e2"

#define ACKBAR_BLE_CHARACTERISTIC_BOARD_NAME "dd12f344-e102-4328-a5fa-64358855cc45"
#define ACKBAR_BLE_CHARACTERISTIC_TRIGGER_COUNT "c13d09a1-5c35-49ec-b1f2-04f344bd967f"

class AckbarBLE : public AckbarEventConsumer
{
  public:
    AckbarBLE();
    ~AckbarBLE();
    char * name();
    bool begin();
    bool calibrate();
    void handleEvent(AckbarEvent * e);

  private:
    BLEServer       *pServer      = nullptr;
    BLEService      *pService     = nullptr;
    BLEAdvertising  *pAdvertising = nullptr;

    std::map<String,BLECharacteristic *> characteristics;
    int triggerCount;
};

#endif // __ACKBAR_BLE_H__