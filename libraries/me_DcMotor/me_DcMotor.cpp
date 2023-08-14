// Working with DC motor.

/*
  Version: 3
  Last mod.: 2023-08-13
*/

#include "me_DcMotor.h"

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

DcMotor::DcMotor(TMotorPins aMotorPins)
{
  MotorPins = aMotorPins;

  pinMode(MotorPins.ForwardPin, OUTPUT);
  pinMode(MotorPins.BackwardPin, OUTPUT);

  DesiredSpeed = 0;
  ActualSpeed = 0;

  SetDesiredSpeed(DesiredSpeed);
}

void DcMotor::SetDesiredSpeed(int8_t aDesiredSpeed)
{
  DesiredSpeed = aDesiredSpeed;

  DesiredSpeed = constrain(DesiredSpeed, -100, 100);

  Update();
}

int8_t DcMotor::GetDesiredSpeed()
{
  return DesiredSpeed;
}

void DcMotor::Update()
{
  const uint8_t MaxSpeedIncrement = 2;

  ActualSpeed = Smooth(ActualSpeed, DesiredSpeed, MaxSpeedIncrement);

  Actualize();
}

int8_t DcMotor::GetActualSpeed()
{
  return ActualSpeed;
}

// -= private =-

void DcMotor::Actualize()
/*
  Set motor speed to <ActualSpeed>.

  On hardware level we map 1D speed vector to two pin values:
  forward pin and backward pin. Full forward (100) maps to (255, 0),
  full backward (-100) maps to (0, 255).
*/
{
  uint8_t MotorPower;

  MotorPower = map(abs(ActualSpeed), 0, 100, 0, 255);

  uint8_t ForwardPower, BackwardPower;

  if (ActualSpeed < 0)
  {
    ForwardPower = 0;
    BackwardPower = MotorPower;
  }
  else
  {
    ForwardPower = MotorPower;
    BackwardPower = 0;
  }

  analogWrite(MotorPins.ForwardPin, ForwardPower);
  analogWrite(MotorPins.BackwardPin, BackwardPower);
}

/*
  2022-09-21
  2023-06-04
  2023-08-13
*/
