#include "AckbarBLE.h"

#include <BLEUtils.h>


AckbarBLE::AckbarBLE()
{
  deliverEventsSynchronously = true;
  triggerCount = 0;
}

AckbarBLE::~AckbarBLE()
{

}

char * AckbarBLE::name()
{
  return("Bluetooth Low Energy");
}


bool AckbarBLE::begin()
{
  BLEDevice::init(configuration->board_name);

  pServer   = BLEDevice::createServer();
  pService  = pServer->createService(ACKBAR_BLE_SERVICE_UUID);

  characteristics[ACKBAR_BLE_CHARACTERISTIC_BOARD_NAME] = pService->createCharacteristic(
    ACKBAR_BLE_CHARACTERISTIC_BOARD_NAME,
    BLECharacteristic::PROPERTY_READ
  );

  characteristics[ACKBAR_BLE_CHARACTERISTIC_BOARD_NAME]->setValue(configuration->board_name);

  characteristics[ACKBAR_BLE_CHARACTERISTIC_TRIGGER_COUNT] = pService->createCharacteristic(
    ACKBAR_BLE_CHARACTERISTIC_TRIGGER_COUNT,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  characteristics[ACKBAR_BLE_CHARACTERISTIC_TRIGGER_COUNT]->setValue(triggerCount);


  pService->start();

  pAdvertising = BLEDevice::getAdvertising();

  pAdvertising->addServiceUUID(ACKBAR_BLE_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();

  return true;
}

bool AckbarBLE::calibrate()
{
  return true;
}

void AckbarBLE::handleEvent(AckbarEvent * e)
{
  if(e->eventType == e->TRAP_EVENT)
  {
    triggerCount++;
    characteristics[ACKBAR_BLE_CHARACTERISTIC_TRIGGER_COUNT]->setValue(triggerCount);
    characteristics[ACKBAR_BLE_CHARACTERISTIC_TRIGGER_COUNT]->notify();
  }
}