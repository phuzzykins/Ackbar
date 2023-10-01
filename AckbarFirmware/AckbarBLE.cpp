#include "AckbarBLE.h"

#include <BLEUtils.h>


char * AckbarBLE::name()
{
  return("Bluetooth Low Energy");
}


void AckbarBLE::begin()
{
  BLEDevice::init(configuration->board_name);

  pServer   = BLEDevice::createServer();
  pService  = pServer->createService(ACKBAR_BLE_SERVICE_UUID);

  characteristics[ACKBAR_BLE_CHARACTERISTIC_BOARD_NAME] = pService->createCharacteristic(
    ACKBAR_BLE_CHARACTERISTIC_BOARD_NAME,
    BLECharacteristic::PROPERTY_READ
  );

  characteristics[ACKBAR_BLE_CHARACTERISTIC_BOARD_NAME]->setValue(configuration->board_name);

  pService->start();

  pAdvertising = BLEDevice::getAdvertising();

  pAdvertising->addServiceUUID(ACKBAR_BLE_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();
}

void AckbarBLE::calibrate()
{

}

void AckbarBLE::handleEvent(AckbarEvent * e)
{

}