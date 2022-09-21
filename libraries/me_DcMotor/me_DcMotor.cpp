#include "me_DcMotor.h"

DcMotor::DcMotor(t_motor_pins motorPins)
{
  this->pins = motorPins;

  pinMode(pins.Forward, OUTPUT);
  pinMode(pins.Backward, OUTPUT);

  Stop();
  SetScaling(1.0);
}

void DcMotor::Stop()
{
  Power = 0;
  Actualize();
}

void DcMotor::SetScaling(float scale)
{
  PowerScale = constrain(scale, -1.0, 1.0);

  Actualize();
}

void DcMotor::SetDirectedPower(int8_t DirectedPower)
{
  DirectedPower = constrain(DirectedPower, -100, 100);
  Power = map(DirectedPower, -100, 100, -128, 127);

  Actualize();
}

void DcMotor::Actualize()
{
  if (Power == 0)
  {
    digitalWrite(pins.Forward, LOW);
    digitalWrite(pins.Backward, LOW);
  }
  else if (IsBackward())
  {
    digitalWrite(pins.Forward, LOW);
    analogWrite(pins.Backward, GetPwmPower());
  }
  else
  {
    digitalWrite(pins.Backward, LOW);
    analogWrite(pins.Forward, GetPwmPower());
  }
}

bool DcMotor::IsBackward()
{
  return (Power * PowerScale < 0);
}

uint8_t DcMotor::GetPwmPower()
{
  int8_t ScaledPower = GetScaledPower();
  if (ScaledPower < 0)
    return map(ScaledPower, -128, 0, 255, 0);
  else
    return map(ScaledPower, 0, 127, 0, 255);
}

int8_t DcMotor::GetScaledPower()
{
  return Power * GetScaling();
}

float DcMotor::GetScaling()
{
  return PowerScale;
}
