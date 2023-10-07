#include "AckbarNotifier.h"
#include "AckbarWifiConnection.h"
#include "AckbarEventService.h"

#include <WiFi.h>
#include <HTTPClient.h>

char * AckbarNotifier::name()
{
  return("NTFY Notifier");
}


bool AckbarNotifier::begin()
{
  missedTrapNotifications = 0;
  return true;
}

bool AckbarNotifier::calibrate()
{
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");

  uri = "https://ntfy.sh/ACK_" + macAddress;

  Serial.print("Notification URL: ");
  Serial.println(uri);

  AckbarEventService s;
  AckbarLinkEvent * e = new AckbarLinkEvent(uri);

  s.publishEvent(e);

  return true;
}

void AckbarNotifier::handleEvent(AckbarEvent * e)
{
  if(e->eventType == e->TRAP_EVENT)
  {
    missedTrapNotifications++;
    AckbarWifiConnection w(configuration);

    if(w.connect())
    {
      WiFiClientSecure client;
      HTTPClient http;

      client.setInsecure();

      http.begin(client, uri);

      http.addHeader("Content-Type", "text/plain");

      int missed = missedTrapNotifications - 1;

      String request("Notification From ");
      request += configuration->board_name;
      request += ": the trap was triggered!";

      if(missed > 0)
      {
        request += " There were ";
        request += missed;
        request += " earlier triggers for which notifications could not be sent.";
      }

      int response = http.POST(request);

      Serial.print("HTTP Response status code: ");
      Serial.println(response);

      if(response == 200)
      {
        missedTrapNotifications = 0;
      }

      http.end();
      client.stop();
    }
    else
    {
      Serial.printf("%s WiFi connection failed\n", name());
    }
  }
}