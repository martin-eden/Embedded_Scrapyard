// Interface to DC motor.

/*
  Version: 3
  Last mod.: 2023-08-13
*/

/*
  Motor speed is represented as 1D vector.
  Value is signed byte: [-128, 127].
  Internally it's clamped to [-100, 100].
  (-100) - full backward, (100) - full forward.
*/

#pragma once

#include <Arduino.h>

struct TMotorPins
{
  int ForwardPin;
  int BackwardPin;
};

class DcMotor
{
  public:
    DcMotor(TMotorPins aMotorPins);

    void SetDesiredSpeed(int8_t aDesiredSpeed);
    int8_t GetDesiredSpeed();

    // Call it every tick to gradually reach desired speed.
    void Update();

    int8_t GetActualSpeed();

  private:
    TMotorPins MotorPins;

    int8_t DesiredSpeed;
    int8_t ActualSpeed;

    void Actualize();
};

/*
  2023-06-04
  2023-08-13 - 1D vector concept, Update()
*/
