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

    void SetPower(uint8_t Power);
    uint8_t GetPower();
    void SetDirection(uint8_t Direction);
    uint8_t GetDirection();

  private:
    TMotorPins Motor;

    uint8_t Power;
    uint8_t OfficialDirection;
    bool IsBackward;

    void Actualize();
};
