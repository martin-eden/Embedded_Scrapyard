#include "me_TwoDcMotorsDirector.h"

TwoDcMotorsDirector::TwoDcMotorsDirector(DcMotor *aLeftMotor, DcMotor *aRightMotor)
{
  LeftMotor = aLeftMotor;
  RightMotor = aRightMotor;

  SetDirection(0);
  SetPower(0);
}

void TwoDcMotorsDirector::SetDirection(int16_t aDirection)
{
  // Official direction is angle in degrees [low(int16_t), high(int16_t)].
  OfficialDirection = aDirection;

  // Internal direction is angle in degrees [0, 359]:
  DirectionDeg = aDirection % 360;
  if (DirectionDeg < 0)
    DirectionDeg += 360;

  Actualize();
}

int16_t TwoDcMotorsDirector::GetDirection()
{
  return OfficialDirection;
}

void TwoDcMotorsDirector::SetPower(uint8_t aPower)
{
  // Official power is byte [0, 255]:
  Power = aPower;

  Actualize();
}

float sign(float a)
{
  if (a < 0)
    return -1.0;
  else if (a > 0)
    return 1.0;
  else
    return 0.0;
}

void TwoDcMotorsDirector::Actualize()
{

  /*
                       Dir = 90
                          |
                     SkidTurnLeft
                          |
    Dir = 180 - Backward -+- Forward - Dir = 0
                          |
                     SkidTurnRight
                          |
                      Dir = 270
  */
  float SinedDirection = (float) sin(DEG_TO_RAD * DirectionDeg); // [0, 1]
  float CosinedDirection = (float) cos(DEG_TO_RAD * DirectionDeg); // [0, 1]

  float Longitudinal = sq(CosinedDirection) * Power; // [0, Power]
  float Lateral = sq(SinedDirection) * Power; // [0, Power]
  // ^ Assert: Longitudinal + Lateral == Power

  float LeftMotorPower, RightMotorPower;
  LeftMotorPower = Longitudinal * sign(CosinedDirection) - Lateral * sign(SinedDirection);
  RightMotorPower = Longitudinal * sign(CosinedDirection) + Lateral * sign(SinedDirection);

  LeftMotor->SetIsBackward((LeftMotorPower < 0));
  LeftMotor->SetPower((uint8_t) abs(LeftMotorPower));

  RightMotor->SetIsBackward((RightMotorPower < 0));
  RightMotor->SetPower((uint8_t) abs(RightMotorPower));
}
