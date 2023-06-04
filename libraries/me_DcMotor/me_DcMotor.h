// Interface to DC motor.

/*
  Version: 2
  Last mod.: 2023-06-04
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
    DcMotor(TMotorPins Motor);

    // Power is interanlly limited to [-100, 100].
    void SetPower(int8_t Power);
    int8_t GetPower();

  private:
    TMotorPins Motor;

    int8_t Power;

    void Actualize();
};
