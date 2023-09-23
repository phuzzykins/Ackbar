#include "AckbarBarrelMechanism.h"
#include "AckbarPins.h"

#define STEPPER_MICROSTEPS      8
#define STEPPER_STEPS_PER_REV   750
#define RUN_CURRENT_PERCENT     100
#define HOLD_CURRENT_PERCENT    25
#define STALL_GUARD_THRESHOLD   0

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

  Serial2.begin(115200, SERIAL_8N1, STP_UART_RX, STP_UART_TX);

  stepperDriver->setup(Serial2);

  stepperDriver->setMicrostepsPerStep(STEPPER_MICROSTEPS);

  stepperDriver->setRunCurrent(RUN_CURRENT_PERCENT);
  stepperDriver->setHoldCurrent(HOLD_CURRENT_PERCENT);
  stepperDriver->setStandstillMode(stepperDriver->STRONG_BRAKING);
  stepperDriver->setStallGuardThreshold(STALL_GUARD_THRESHOLD);
  stepperDriver->disableStealthChop();
  stepperDriver->disableCoolStep();
  stepperDriver->enable();

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
