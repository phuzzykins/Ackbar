#ifndef __ACKBAR_WIFI_CONNECTION_H__
#define __ACKBAR_WIFI_CONNECTION_H__

#include "AckbarConfiguration.h"


class AckbarWifiConnection
{
  public:
    AckbarWifiConnection(AckbarConfiguration * c);
    ~AckbarWifiConnection();
    bool connect();
    void disconnect();

  private:
    AckbarConfiguration * configuration = nullptr;
};


#endif // __ACKBAR_WIFI_CONNECTION_H__
