#include "me_DcMotor.h"

DcMotor::DcMotor(TMotorPins _Motor)
{
  Motor = _Motor;

  SetPower(0);
  SetIsBackward(false);
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

void DcMotor::SetIsBackward(bool aIsBackward)
{
  IsBackward = aIsBackward;

  Actualize();
}

bool DcMotor::GetIsBackward()
{
  return IsBackward;
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

  analogWrite(Motor.ForwardPin, ForwardPower);
  analogWrite(Motor.BackwardPin, BackwardPower);
}
