#include "AckbarNotifier.h"
#include "AckbarWifiConnection.h"
#include "AckbarEventService.h"

#include <WiFi.h>
#include <HTTPClient.h>

char * AckbarNotifier::name()
{
  return("NTFY Notifier");
}


void AckbarNotifier::begin()
{

}

void AckbarNotifier::calibrate()
{
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");

  uri = "http://ntfy.sh/ACK_" + macAddress;

  Serial.print("Notification URL: ");
  Serial.println(uri);

  AckbarEventService s;
  AckbarLinkEvent * e = new AckbarLinkEvent(uri);

  s.publishEvent(e);
}

void AckbarNotifier::handleEvent(AckbarEvent * e)
{
  if(e->eventType == e->TRAP_EVENT)
  {
    AckbarWifiConnection w(configuration);

    if(w.connect())
    {
      WiFiClient client;
      HTTPClient http;

      http.begin(client, uri);

      http.addHeader("Content-Type", "text/plain");

      String request("Notification From ");
      request += configuration->board_name;
      request += ": the trap was triggered!";

      int response = http.POST(request);

      Serial.print("HTTP Response status code: ");
      Serial.println(response);

      http.end();
      client.stop();
    }
    else
    {
      Serial.printf("%s WiFi connection failed\n", name());
    }
  }
}