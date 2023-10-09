// Working with one DC motor via Deek motor shield.

/*
  Version: 1
  Last mod.: 2023-10-07
*/

#include "me_DeekMotor.h"

// -= handy =-

template <typename type> int8_t sign(type value)
{
  return type((value > 0) - (value < 0));
}

int8_t Smooth(int8_t CurrentValue, int8_t DesiredValue, uint8_t MaxValueChange)
{
  int16_t ValueDelta = (DesiredValue - CurrentValue);

  if (abs(ValueDelta) > MaxValueChange)
  {
    ValueDelta = MaxValueChange * sign(ValueDelta);
  }

  int8_t Result = (int16_t) (CurrentValue + ValueDelta);

  return Result;
}

// -= public =-

DeekMotor::DeekMotor(TDeekMotorPins aMotorPins)
{
  MotorPins = aMotorPins;

  pinMode(MotorPins.Direction_Pin, OUTPUT);
  pinMode(MotorPins.Pwm_Pin, OUTPUT);
  pinMode(MotorPins.Brake_Pin, OUTPUT);

  digitalWrite(MotorPins.Direction_Pin, LOW);
  digitalWrite(MotorPins.Pwm_Pin, LOW);
  digitalWrite(MotorPins.Brake_Pin, LOW);

  DesiredSpeed = 0;
  ActualSpeed = 0;

  SetDesiredSpeed(DesiredSpeed);
}

void DeekMotor::SetDesiredSpeed(int8_t aDesiredSpeed)
{
  DesiredSpeed = aDesiredSpeed;

  DesiredSpeed = constrain(DesiredSpeed, -100, 100);

  Update();
}

int8_t DeekMotor::GetDesiredSpeed()
{
  return DesiredSpeed;
}

void DeekMotor::Update()
{
  const uint8_t MaxSpeedIncrement = 2;

  ActualSpeed = Smooth(ActualSpeed, DesiredSpeed, MaxSpeedIncrement);

  Actualize();
}

int8_t DeekMotor::GetActualSpeed()
{
  return ActualSpeed;
}

// -= private =-

void DeekMotor::Actualize()
/*
  Set motor speed to <ActualSpeed>.

  On hardware level we map 1D speed vector to two pin values:
  forward pin and backward pin. Full forward (100) maps to (255, 0),
  full backward (-100) maps to (0, 255).
*/
{
  uint8_t MotorPower;

  MotorPower = map(abs(ActualSpeed), 0, 100, 0, 255);

  uint8_t Direction, Pwm;

  Direction = (ActualSpeed < 0) ? HIGH : LOW;
  Pwm = MotorPower;

  digitalWrite(MotorPins.Direction_Pin, Direction);
  analogWrite(MotorPins.Pwm_Pin, Pwm);
}

/*
  2023-08-13 - inherited
  2023-10-07
*/
