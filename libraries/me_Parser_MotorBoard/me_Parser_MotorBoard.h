// Serial() parser for motor board commands.

/*
  Last mod.: 2023-10-14
*/

#pragma once

#include <Arduino.h>

namespace MotorboardCommands
{
  enum TCommandType
  {
    Command_LeftMotor,
    Command_RightMotor,
    Command_Duration
  };

  struct TMotorboardCommand
  {
    TCommandType Type;
    union
    {
      int8_t MotorSpeed;
      int16_t Duration_Ms;
    };
  };

  bool ParseCommand(TMotorboardCommand *Result);
  void DisplayCommand(TMotorboardCommand Command);
}
