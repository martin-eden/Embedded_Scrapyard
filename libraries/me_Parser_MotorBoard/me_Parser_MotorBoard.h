// Serial() parser for motor board commands.

/*
  Version: 2
  Last mod.: 2023-11-09
*/

#pragma once

#include <Arduino.h>

namespace MotorboardCommandsParser
{
  enum TCommandType
  {
    CommandType_LeftMotor,
    CommandType_RightMotor,
    CommandType_Duration
  };

  struct TMotorboardCommand
  {
    TCommandType CommandType;
    union
    {
      int8_t MotorSpeed_Pc;
      uint16_t Duration_Ms;
    };
  };

  bool ParseCommand(TMotorboardCommand *Result);
}

/*
  2023-10-14
  2023-11-09
*/
