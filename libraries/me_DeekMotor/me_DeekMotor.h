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

    void SetDesiredSpeed(int8_t aDesiredSpeed);
    int8_t GetDesiredSpeed();

    // Call it every tick to gradually reach desired speed.
    void Update();

    int8_t GetActualSpeed();

  protected:
    TDeekMotorPins MotorPins;

    int8_t DesiredSpeed;
    int8_t ActualSpeed;

  private:
    void Actualize();
};

/*
  2023-10-07 - started on basis of "me_DcMotor".
*/
