#pragma once

#include <Arduino.h>

struct t_motor_pins
{
  int Forward;
  int Backward;
};

class DcMotor
{
  public:
    DcMotor(t_motor_pins motorPins);

    void Stop();

    void SetDirectedPower(int8_t DirectedPower);
    void SetScaling(float scale);
    float GetScaling();
    int8_t GetScaledPower();

  private:
    t_motor_pins pins;
    int8_t Power = 0;
    bool IsBackward();
    float PowerScale;

    void Actualize();
    void SetPower();
    void SetDirection();
    uint8_t GetPwmPower();
};
