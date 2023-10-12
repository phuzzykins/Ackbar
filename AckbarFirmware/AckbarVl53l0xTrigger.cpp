#include "AckbarVl53l0xTrigger.h"
#include "AckbarPins.h"

#define ANT_DETECTION_THRESHHOLD 10

char * AckbarVl53l0xTrigger::name()
{
  return("Laser Time-of-Flight Trigger");
}

bool AckbarVl53l0xTrigger::begin()
{
  Wire.begin();

  pinMode(TOF_XSHUT, OUTPUT);

  digitalWrite(TOF_XSHUT, LOW);
  delay(10);
  digitalWrite(TOF_XSHUT, HIGH);
  delay(10);

  device.setTimeout(500);

  if(device.init())
  {
    device.setMeasurementTimingBudget(configuration->tof_timing_budget_us);
    return true;
  }
  else
  {
    return false;
  }
}

bool AckbarVl53l0xTrigger::calibrate()
{
  uint16_t min_distance   = -1;
  uint16_t max_distance   = 0;
  uint32_t total_distance = 0;
  uint8_t samples         = 0;

  while(samples < 100)
  {
    uint16_t distance_mm = device.readRangeSingleMillimeters();

    if(device.timeoutOccurred())
    {
      Serial.println("Timeout");
      return false;
    }
    else
    {
      total_distance += distance_mm;
      samples++;
      if(distance_mm > max_distance)
      {
        max_distance = distance_mm;
      }
      if(distance_mm < min_distance)
      {
        min_distance = distance_mm;
      }
      delay(10);
    }
  }

  uint16_t average_distance = (uint16_t)(total_distance / samples);
  nominal_distance_mm = average_distance;

  uint16_t min_variance = average_distance - min_distance;
  uint16_t max_variance = max_distance - average_distance;

  variance_mm         = max(min_variance, max_variance);
  trigger_distance_mm = variance_mm * 3;

  Serial.print("Minimum Sensor Distance: ");
  Serial.print(min_distance);
  Serial.println(" mm");

  Serial.print("Maximum Sensor Distance: ");
  Serial.print(max_distance);
  Serial.println(" mm");

  Serial.print("Nominal Sensor Distance: ");
  Serial.print(nominal_distance_mm);
  Serial.println(" mm");

  Serial.print("Sensor Variance: ");
  Serial.print(variance_mm);
  Serial.println(" mm");

  Serial.print("Trigger Distance: ");
  Serial.print(trigger_distance_mm);
  Serial.println(" mm");

  return true;
}

bool AckbarVl53l0xTrigger::isReady()
{
  consecutiveReadings = 0;

  uint16_t distance_mm = device.readRangeSingleMillimeters();

  if(device.timeoutOccurred())
  {
    Serial.println("Timeout");
    return false;
  }
  else
  {
    if(distance_mm > (nominal_distance_mm + variance_mm * 2))
    {
      Serial.print("Sensor distance out of range: ");
      Serial.print(distance_mm);
      Serial.println(" mm");
      return false;
    }
    else if(distance_mm > (nominal_distance_mm - variance_mm * 1.5))
    {
      Serial.print("Sensor distance within range: ");
      Serial.print(distance_mm);
      Serial.println(" mm");
      return true;
    }
    else
    {
      Serial.print("Waiting for distance sensor to stabilize: ");
      Serial.print(distance_mm);
      Serial.println(" mm");
      return false;
    }
  }
}

bool AckbarVl53l0xTrigger::isTriggered()
{
  uint16_t distance_mm = device.readRangeSingleMillimeters();

  if(device.timeoutOccurred())
  {
    Serial.println("Timeout");
  }
  else
  {
    if(distance_mm < ANT_DETECTION_THRESHHOLD)
    {
      consecutiveReadings = 0;
    }
    else if(distance_mm < (nominal_distance_mm - trigger_distance_mm))
    {
      consecutiveReadings++;
    }
    else
    {
      consecutiveReadings = 0;
    }
  }

  return(consecutiveReadings > 2);
}
