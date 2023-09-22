#ifndef __ACKBAR_CALIBRATION_H__
#define __ACKBAR_CALIBRATION_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <FFat.h>

#define BOARD_ID_LENGTH 16

class Ackbar::Calibration
{
  public:
	  char      board_id[BOARD_ID_LENGTH];
	  uint16_t  tof_trigger_distance_mm;
	  uint16_t  tof_nominal_distance_mm;
	  uint16_t  tof_variance_mm;
	  int8_t    wifi_rssi;
	  bool      has_tof;
	  bool      has_gnss;
};

#endif // __ACKBAR_CALIBRATION_H__