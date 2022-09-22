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
    void SetIsBackward(bool IsBackward);
    bool GetIsBackward();

  private:
    TMotorPins Motor;

    uint8_t Power;
    bool IsBackward;

    void Actualize();
};
