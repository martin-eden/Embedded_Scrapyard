#include "me_DcMotor.h"

DcMotor::DcMotor(TMotorPins _Motor)
{
  Motor = _Motor;

  SetPower(0);
  SetDirection(0);
}

void DcMotor::SetPower(uint8_t aPower)
{
  Power = aPower;

  Actualize();
}

uint8_t DcMotor::GetPower()
{
  return Power;
}

void DcMotor::SetDirection(uint8_t aDirection)
{
  // Official direction is integer [0, 255].
  // Even numbers for forward, odd for backward.
  OfficialDirection = aDirection;

  // Internally we just need backward flag for custom logic.
  IsBackward = (OfficialDirection % 2);

  Actualize();
}

uint8_t DcMotor::GetDirection()
{
  return OfficialDirection;
}

void DcMotor::Actualize()
{
  uint8_t ForwardPower, BackwardPower;

  if (IsBackward)
  {
    ForwardPower = 0;
    BackwardPower = Power;
  }
  else
  {
    ForwardPower = Power;
    BackwardPower = 0;
  }

  pinMode(Motor.ForwardPin, OUTPUT);
  pinMode(Motor.BackwardPin, OUTPUT);

  analogWrite(MotorPins.ForwardPin, ForwardPower);
  analogWrite(MotorPins.BackwardPin, BackwardPower);
}
