#include "AckbarWifiConnection.h"
#include "AckbarEventService.h"
#include <WiFi.h>

// Initial implementation is not thread safe

AckbarWifiConnection::AckbarWifiConnection(AckbarConfiguration * c)
{
  configuration = c;
}

bool AckbarWifiConnection::connect()
{
  if(strlen(configuration->wifi_ssid) == 0)
  {
    Serial.println("WiFi is not configured");
    return false;
  }

  Serial.println("Connecting to WiFi");

  int timeout_remaining = configuration->wifi_timeout_ms;
  int timeout_at        = millis() + configuration->wifi_timeout_ms;

  while(millis() < timeout_at)
  {
    WiFi.begin(configuration->wifi_ssid, configuration->wifi_password);

    int timeout_remaining = timeout_at - millis();

    int start             = millis();
    uint8_t status        = WiFi.waitForConnectResult(timeout_remaining);
    int elapsed           = millis() - start;

    Serial.printf("Waited %d ms for WiFi connection result\n", elapsed);

    if(status == WL_CONNECTED)
    {
      Serial.printf("Connected to WiFi: %s\n",      configuration->wifi_ssid);
      Serial.printf("IP address:        %s\n",      WiFi.localIP().toString().c_str());
      Serial.printf("Signal Strength:   %i dBm\n",  WiFi.RSSI());

      {
        AckbarEventService s;
        AckbarWifiConnectionEvent * e = new AckbarWifiConnectionEvent(WiFi.localIP(), WiFi.RSSI());
        s.publishEvent(e);
      };

      return true;
    }
    else
    {
      Serial.printf("WiFi connection failed, wl_status_t code: %d\n", status);

      delay(50);
    }
  }
  return false;
}

void AckbarWifiConnection::disconnect()
{
  Serial.println("Disconnecting from WiFi");
  WiFi.disconnect(true);
  {
    AckbarEventService s;
    AckbarWifiDisconnectionEvent * e = new AckbarWifiDisconnectionEvent();
    s.publishEvent(e);
  };
}

AckbarWifiConnection::~AckbarWifiConnection()
{
  disconnect();
}