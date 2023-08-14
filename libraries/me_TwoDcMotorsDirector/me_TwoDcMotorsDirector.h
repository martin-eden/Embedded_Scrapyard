// Direction-power handler for two DC motors.

/*
  Version: 2
  Last mod.: 2023-08-13
*/

#pragma once

#include <Arduino.h>
#include <me_DcMotor.h>

class TwoDcMotorsDirector
{
  public:
    TwoDcMotorsDirector(DcMotor *LeftMotor, DcMotor *RightMotor);

    void SetDirection(int16_t Direction); // direction in degrees
    int16_t GetDirection();
    void SetPower(uint8_t Power); // power [0, 255]
    uint8_t GetPower();
    void Update(); // call it every tick to gradually set power

  private:
    DcMotor *LeftMotor, *RightMotor;

    int16_t OfficialDirection;
    int16_t DirectionDeg;
    uint8_t Power;

    void Actualize();
};

/*
  2023-06-03
*/
