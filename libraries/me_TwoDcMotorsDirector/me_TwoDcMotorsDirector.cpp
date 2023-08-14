// Mapping diraction angle and power to actual PWMs for two DC motors.

/*
  Version: 3
  Last mod.: 2023-08-13
*/

#include "me_TwoDcMotorsDirector.h"

#include <me_Math_Scaling.h>

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

uint8_t TwoDcMotorsDirector::GetPower()
{
  return Power;
}

void TwoDcMotorsDirector::Update()
{
  LeftMotor->Update();
  RightMotor->Update();
}

// -= private =-

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
  float DirectionSin = (float) sin(radians(DirectionDeg)); // [0, 1]
  float DirectionCos = (float) cos(radians(DirectionDeg)); // [0, 1]

  float Longitudinal = sq(DirectionCos) * Power; // [0, Power]
  float Lateral = sq(DirectionSin) * Power; // [0, Power]
  // ^ Assert: Longitudinal + Lateral == Power

  float LeftMotorPower, RightMotorPower;
  LeftMotorPower = Longitudinal * sign(DirectionCos) - Lateral * sign(DirectionSin);
  RightMotorPower = Longitudinal * sign(DirectionCos) + Lateral * sign(DirectionSin);

  LeftMotorPower = me_Math_Scaling::FloatMap(LeftMotorPower, -255, 255, -100, 100);
  RightMotorPower = me_Math_Scaling::FloatMap(RightMotorPower, -255, 255, -100, 100);

  LeftMotor->SetDesiredSpeed(LeftMotorPower);
  RightMotor->SetDesiredSpeed(RightMotorPower);
}

/*
  2023-06-03
  2023-08-13
*/
