#ifndef __ACKBAR_CONFIGURATION_H__
#define __ACKBAR_CONFIGURATION_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <FFat.h>

#define BOARD_NAME_LENGTH       32
#define WIFI_SSID_LENGTH        32
#define WIFI_PASSWORD_LENGTH    32

class Ackbar::Configuration
{
  public:

  protected:
	  char      board_name[BOARD_NAME_LENGTH];
	  uint16_t  motor_rpm;
	  uint16_t  motor_acceleration;
	  uint16_t  motor_deceleration;
	  uint32_t  tof_timing_budget_us;
	  char      wifi_ssid[WIFI_SSID_LENGTH];
	  char      wifi_password[WIFI_PASSWORD_LENGTH];
	  uint32_t  wifi_timeout_ms;
	  double    home_latitude;
	  double    home_longitude;
	  uint32_t  geofence_distance_m;
	  bool      require_wifi;
	  bool      require_gnss;
};

#endif // __ACKBAR_CONFIGURATION_H__