// Interface to DC motor in Deek motor shield.

/*
  Version: 1
  Last mod.: 2023-10-07
*/

/*
  Motor speed clamped to range [-100, 100].
  (-100) - full backward, (100) - full forward.
*/

#pragma once

#include <Arduino.h>

struct TDeekMotorPins
{
  uint8_t Direction_Pin;
  uint8_t Pwm_Pin;
  uint8_t Brake_Pin;
};

class DeekMotor
{
  public:
    DeekMotor(TDeekMotorPins aMotorPins);

    void SetSpeed(int8_t aDesiredSpeed);
    int8_t GetSpeed();

  protected:
    TDeekMotorPins MotorPins;

    int8_t Speed;

  private:
    void Actualize();
};

/*
  2023-10-07 - started on basis of "me_DcMotor".
*/
