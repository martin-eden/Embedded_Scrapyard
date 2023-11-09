// Serial() parser for motor board commands.

/*
  Status: stable
  Version: 3
  Last mod.: 2023-11-09
*/

/*
  Motor board command format.

  Command is two tokens. Command type and command value.

  "L" [-100, 100] -- Left motor. Set specified power and direction.
  "R" [-100, 100] -- Right motor. Set specified power and direction.
  "D" [0, 5000] -- Delay for given value in milliseconds.

  Whitespaces are stripped, so "L 50 R -50 D 1000" == "L50R50D1000".
*/

#include "me_Parser_MotorBoard.h"

using namespace MotorboardCommandsParser;

// ---

bool HasWordToken()
{
  while(Serial.available())
  {
    char NextChar = Serial.peek();

    if ((NextChar >= 'A') || (NextChar <= 'z'))
    {
      return true;
    }

    Serial.read();
  }

  return false;
}

// Convert command name to command type. Reads Serial.
bool GetCommandType(TCommandType *CommandType)
{
  // Command names.
  const char
    CommandName_LeftMotor = 'L',
    CommandName_RightMotor = 'R',
    CommandName_Duration = 'D';

  if (!Serial.available())
  {
    return false;
  }

  char NextChar = Serial.read();

  switch (NextChar)
  {
    case CommandName_LeftMotor:
      *CommandType = CommandType_LeftMotor;
      break;

    case CommandName_RightMotor:
      *CommandType = CommandType_RightMotor;
      break;

    case CommandName_Duration:
      *CommandType = CommandType_Duration;
      break;

    default:
      return false;
      break;
  }

  return true;
}

// Get motor value from Serial.
bool GetMotorValue(int8_t *MotorValue)
{
  const int8_t
    MinValue = -100,
    MaxValue = 100;

  bool Result = false;

  if (!Serial.available())
  {
    return false;
  }

  Result = true;

  LookaheadMode lookaheadMode = SKIP_WHITESPACE;
  int32_t ParsedInt = Serial.parseInt(lookaheadMode);

  if ((ParsedInt >= MinValue) && (ParsedInt <= MaxValue))
  {
    *MotorValue = ParsedInt;
  }
  else
  {
    Result = false;
  }

  return Result;
}

// Get duration value (ms) from Serial.
bool GetDurationValue(uint16_t *DurationValue)
{
  const int16_t
    MinValue = 0,
    MaxValue = 5000;

  bool Result = false;

  if (!Serial.available())
  {
    return false;
  }

  Result = true;

  LookaheadMode lookaheadMode = SKIP_WHITESPACE;
  int32_t ParsedInt = Serial.parseInt(lookaheadMode);

  if ((ParsedInt >= MinValue) && (ParsedInt <= MaxValue))
  {
    *DurationValue = ParsedInt;
  }
  else
  {
    Result = false;
  }

  return Result;
}

// ---

/*
  Parse Serial for valid command.

  Core function.
*/
bool MotorboardCommandsParser::ParseCommand(TMotorboardCommand * Result)
{
  if (!Serial.available())
    return false;

  if (!HasWordToken())
    return false;

  if (!GetCommandType(&Result->CommandType))
    return false;

  switch (Result->CommandType)
  {
    case CommandType_LeftMotor:
    case CommandType_RightMotor:
      if (!GetMotorValue(&Result->MotorSpeed_Pc))
        return false;

      break;

    case CommandType_Duration:
      if (!GetDurationValue(&Result->Duration_Ms))
        return false;

      break;

    default:
      return false;
  }

  return true;
}

/*
  2023-10-14
  2023-11-05
  2023-11-09
*/
