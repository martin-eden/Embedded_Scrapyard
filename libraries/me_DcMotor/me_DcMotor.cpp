// Working with DC motor.

/*
  Version: 2
  Last mod.: 2023-06-04
*/

#include "me_DcMotor.h"

DcMotor::DcMotor(TMotorPins _Motor)
{
  Motor = _Motor;

  pinMode(Motor.ForwardPin, OUTPUT);
  pinMode(Motor.BackwardPin, OUTPUT);

  SetPower(0);
}

template <typename type> int8_t sign(type value)
{
  return type((value > 0) - (value < 0));
}

int8_t GetSmoothedPower(int8_t CurrentPower, int8_t RequestedPower, uint8_t MaxPowerChange)
{
  int16_t PowerDelta = (RequestedPower - CurrentPower);
  if (abs(PowerDelta) > MaxPowerChange)
  {
    PowerDelta = MaxPowerChange * sign(PowerDelta);
  }

  int8_t Result = (int16_t) (CurrentPower + PowerDelta);

  return Result;
}

void DcMotor::SetPower(int8_t aPower)
{
  const uint8_t MaxPowerIncrement = 2;

  aPower = constrain(aPower, -100, 100);

  Power = GetSmoothedPower(Power, aPower, MaxPowerIncrement);

  Actualize();
}

int8_t DcMotor::GetPower()
{
  return Power;
}

void DcMotor::Actualize()
{
  uint8_t MotorPower;
  uint8_t ForwardPower, BackwardPower;

  MotorPower = map(abs(Power), 0, 100, 0, 255);

  ForwardPower = 0;
  BackwardPower = 0;

  if (Power < 0)
    BackwardPower = MotorPower;
  else
    ForwardPower = MotorPower;

  analogWrite(Motor.ForwardPin, ForwardPower);
  analogWrite(Motor.BackwardPin, BackwardPower);
}

/*
  2022-09-21
  2023-06-04
*/
