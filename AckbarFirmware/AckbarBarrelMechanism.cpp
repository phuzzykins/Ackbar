#include "AckbarBarrelMechanism.h"
#include "AckbarPins.h"

#define STEPPER_MICROSTEPS      8
#define STEPPER_STEPS_PER_REV   750
#define RUN_CURRENT_PERCENT     100
#define HOLD_CURRENT_PERCENT    25
#define STALL_GUARD_THRESHOLD   0

char * AckbarBarrelMechanism::name()
{
  return("Barrel Mechanism");
}

AckbarBarrelMechanism::~AckbarBarrelMechanism()
{
  if(stepperMotor != nullptr)
  {
    delete stepperMotor;
  }

  if(stepperDriver != nullptr)
  {
    delete stepperDriver;
  }
}

void AckbarBarrelMechanism::getDriverSettings()
{
  Serial.println("*************************");
  Serial.println("getSettings()");
  TMC2209::Settings settings = stepperDriver->getSettings();
  Serial.print("settings.is_communicating = ");
  Serial.println(settings.is_communicating);
  Serial.print("settings.is_setup = ");
  Serial.println(settings.is_setup);
  Serial.print("settings.software_enabled = ");
  Serial.println(settings.software_enabled);
  Serial.print("settings.microsteps_per_step = ");
  Serial.println(settings.microsteps_per_step);
  Serial.print("settings.inverse_motor_direction_enabled = ");
  Serial.println(settings.inverse_motor_direction_enabled);
  Serial.print("settings.stealth_chop_enabled = ");
  Serial.println(settings.stealth_chop_enabled);
  Serial.print("settings.standstill_mode = ");
  switch (settings.standstill_mode)
  {
    case TMC2209::NORMAL:
      Serial.println("normal");
      break;
    case TMC2209::FREEWHEELING:
      Serial.println("freewheeling");
      break;
    case TMC2209::STRONG_BRAKING:
      Serial.println("strong_braking");
      break;
    case TMC2209::BRAKING:
      Serial.println("braking");
      break;
  }
  Serial.print("settings.irun_percent = ");
  Serial.println(settings.irun_percent);
  Serial.print("settings.irun_register_value = ");
  Serial.println(settings.irun_register_value);
  Serial.print("settings.ihold_percent = ");
  Serial.println(settings.ihold_percent);
  Serial.print("settings.ihold_register_value = ");
  Serial.println(settings.ihold_register_value);
  Serial.print("settings.iholddelay_percent = ");
  Serial.println(settings.iholddelay_percent);
  Serial.print("settings.iholddelay_register_value = ");
  Serial.println(settings.iholddelay_register_value);
  Serial.print("settings.automatic_current_scaling_enabled = ");
  Serial.println(settings.automatic_current_scaling_enabled);
  Serial.print("settings.automatic_gradient_adaptation_enabled = ");
  Serial.println(settings.automatic_gradient_adaptation_enabled);
  Serial.print("settings.pwm_offset = ");
  Serial.println(settings.pwm_offset);
  Serial.print("settings.pwm_gradient = ");
  Serial.println(settings.pwm_gradient);
  Serial.print("settings.cool_step_enabled = ");
  Serial.println(settings.cool_step_enabled);
  Serial.print("settings.analog_current_scaling_enabled = ");
  Serial.println(settings.analog_current_scaling_enabled);
  Serial.print("settings.internal_sense_resistors_enabled = ");
  Serial.println(settings.internal_sense_resistors_enabled);
  Serial.println("*************************");
  Serial.println();

  Serial.println("*************************");
  Serial.println("hardwareDisabled()");
  bool hardware_disabled = stepperDriver->hardwareDisabled();
  Serial.print("hardware_disabled = ");
  Serial.println(hardware_disabled);
  Serial.println("*************************");
  Serial.println();

  Serial.println("*************************");
  Serial.println("getStatus()");
  TMC2209::Status status = stepperDriver->getStatus();
  Serial.print("status.over_temperature_warning = ");
  Serial.println(status.over_temperature_warning);
  Serial.print("status.over_temperature_shutdown = ");
  Serial.println(status.over_temperature_shutdown);
  Serial.print("status.short_to_ground_a = ");
  Serial.println(status.short_to_ground_a);
  Serial.print("status.short_to_ground_b = ");
  Serial.println(status.short_to_ground_b);
  Serial.print("status.low_side_short_a = ");
  Serial.println(status.low_side_short_a);
  Serial.print("status.low_side_short_b = ");
  Serial.println(status.low_side_short_b);
  Serial.print("status.open_load_a = ");
  Serial.println(status.open_load_a);
  Serial.print("status.open_load_b = ");
  Serial.println(status.open_load_b);
  Serial.print("status.over_temperature_120c = ");
  Serial.println(status.over_temperature_120c);
  Serial.print("status.over_temperature_143c = ");
  Serial.println(status.over_temperature_143c);
  Serial.print("status.over_temperature_150c = ");
  Serial.println(status.over_temperature_150c);
  Serial.print("status.over_temperature_157c = ");
  Serial.println(status.over_temperature_157c);
  Serial.print("status.current_scaling = ");
  Serial.println(status.current_scaling);
  Serial.print("status.stealth_chop_mode = ");
  Serial.println(status.stealth_chop_mode);
  Serial.print("status.standstill = ");
  Serial.println(status.standstill);
  Serial.println("*************************");
  Serial.println();
}

void AckbarBarrelMechanism::begin()
{
  if(stepperMotor == nullptr)
  {
    stepperMotor  = new BasicStepperDriver(STEPPER_STEPS_PER_REV, STP_DIR, STP_STEP, STP_EN);
  }

  if(stepperDriver == nullptr)
  {
    stepperDriver = new TMC2209();
  }

  pinMode(STP_MS1, OUTPUT);
  pinMode(STP_MS2, OUTPUT);
  pinMode(STP_DIAG, INPUT_PULLUP);
  pinMode(HALL, INPUT_PULLUP);

  //attachInterrupt(digitalPinToInterrupt(STP_DIAG), &stepperDiagInterrupt, FALLING);

  digitalWrite(STP_MS1, LOW);
  digitalWrite(STP_MS2, LOW);

  stepperDriver->setup(Serial2, 115200, TMC2209::SERIAL_ADDRESS_0, STP_UART_RX, STP_UART_TX);

  stepperDriver->setReplyDelay(10000);
  stepperDriver->setMicrostepsPerStep(STEPPER_MICROSTEPS);
  stepperDriver->setRunCurrent(RUN_CURRENT_PERCENT);
  stepperDriver->setHoldCurrent(HOLD_CURRENT_PERCENT);
  stepperDriver->setStandstillMode(stepperDriver->STRONG_BRAKING);
  stepperDriver->setStallGuardThreshold(STALL_GUARD_THRESHOLD);
  stepperDriver->disableStealthChop();
  stepperDriver->disableCoolStep();
  stepperDriver->enable();

  if(stepperDriver->isSetupAndCommunicating())
  {
    //getDriverSettings();
  }
  else
  {
    Serial.println("!!! Stepper driver is not setup and communicating");
  }
  

  stepperMotor->setEnableActiveState(LOW);
  stepperMotor->begin(configuration->motor_rpm, STEPPER_MICROSTEPS);

  stepperMotor->setSpeedProfile(
    stepperMotor->LINEAR_SPEED, 
    configuration->motor_acceleration * STEPPER_STEPS_PER_REV, 
    configuration->motor_deceleration * STEPPER_STEPS_PER_REV
  );

  stepperMotor->disable();
}

bool AckbarBarrelMechanism::limitSwitchTriggered()
{
  return(digitalRead(HALL) == LOW);
}

bool AckbarBarrelMechanism::seekLimit()
{
  int count = 0;

  stepperMotor->enable();
  while((count < 360) && (limitSwitchTriggered() == false))
  {
    count++;
    stepperMotor->rotate(1);
  }
  stepperMotor->disable();

  return limitSwitchTriggered();
}

void AckbarBarrelMechanism::flip()
{
  stepperMotor->enable();
  stepperMotor->rotate(180);
  stepperMotor->disable();
}

void AckbarBarrelMechanism::activate()
{
  flip();
  seekLimit();
}

void AckbarBarrelMechanism::reset()
{
  flip();
}

void AckbarBarrelMechanism::calibrate()
{
  seekLimit();
  flip();
}
