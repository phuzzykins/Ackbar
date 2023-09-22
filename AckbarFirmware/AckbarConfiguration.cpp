#include "AckbarConfiguration.h"
#include <ArduinoJson.h>
#include <Stream.h>

AckbarConfiguration::AckbarConfiguration()
{
  motor_rpm                = 60;
  motor_acceleration       = 2;
  motor_deceleration       = 2;
  tof_timing_budget_us     = 50000;
  trap_dwell_time_ms       = 5000;
  wifi_timeout_ms          = 60000;
  require_wifi             = false;
  require_gnss             = false;
  home_latitude            = 0.0;
  home_longitude           = 0.0;
  geofence_distance_m      = 0;

  strcpy(wifi_ssid,        "");
  strcpy(wifi_password,    "");
  strcpy(board_name,       "Unnamed");
}

struct PsramAllocator {
  void* allocate(size_t size)
  {
    return ps_malloc(size);
  }

  void deallocate(void* pointer)
  {
    free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size)
  {
    return ps_realloc(ptr, new_size);
  }
};

using PsramJsonDocument = BasicJsonDocument<PsramAllocator>;

void AckbarConfiguration::fromStream(Stream& config)
{
  PsramJsonDocument doc(1024);

  DeserializationError error = deserializeJson(doc, config);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());

    Serial.println(config.readString());

    return;
  }

  motor_rpm              = doc["motor_rpm"];
  motor_acceleration     = doc["motor_acceleration"];
  motor_deceleration     = doc["motor_deceleration"];
  tof_timing_budget_us   = doc["tof_timing_budget_us"];
  trap_dwell_time_ms     = doc["trap_dwell_time_ms"];
  wifi_timeout_ms        = doc["wifi_timeout_ms"];
  home_latitude          = doc["home_latitude"];
  home_longitude         = doc["home_longitude"];
  geofence_distance_m    = doc["geofence_distance_m"];
  require_wifi           = doc["require_wifi"];
  require_gnss           = doc["require_gnss"];

  strncpy(board_name,      doc["board_name"],     sizeof(board_name));
  strncpy(wifi_ssid,       doc["wifi_ssid"],      sizeof(wifi_ssid));
  strncpy(wifi_password,   doc["wifi_password"],  sizeof(wifi_password));
}