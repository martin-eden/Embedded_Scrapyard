// Serial() parser for motor board commands.

/*
  Status: stable
  Version: 2
  Last mod.: 2023-11-05
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

using namespace MotorboardCommands;

// For lexer.
enum TTokenType
{
  Token_Word,
  Token_Number
};

// Lexer. Reads Serial().
bool PeekNextToken(TTokenType *NextToken)
{
  bool IsOk = false;

  char IncomingChar;

  while (Serial.available() && !IsOk)
  {
    IncomingChar = Serial.peek();

    if (
      (IncomingChar >= 'A') ||
      (IncomingChar <= 'z')
    ) {
      *NextToken = Token_Word;
      IsOk = true;
    }
    else if (
      (IncomingChar == '-') ||
      ((IncomingChar >= '0') && (IncomingChar <= '9'))
    ) {
      *NextToken = Token_Number;
      IsOk = true;
    }
    else
    {
      Serial.read();
    }
  }

  return IsOk;
}

// Convert command name to command type. Reads Serial.
bool GetCommandType(TCommandType *CommandType)
{
  // Command names.
  const char
    CommandName_LeftMotor = 'L',
    CommandName_RightMotor = 'R',
    CommandName_Duration = 'D';

  bool Result = false;

  if (!Serial.available())
  {
    return false;
  }

  char IncomingChar;
  IncomingChar = Serial.read();

  Result = true;

  switch (IncomingChar)
  {
    case CommandName_LeftMotor:
      *CommandType = Command_LeftMotor;
      break;
    case CommandName_RightMotor:
      *CommandType = Command_RightMotor;
      break;
    case CommandName_Duration:
      *CommandType = Command_Duration;
      break;
    default:
      Result = false;
      break;
  }

  return Result;
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

// Core function. Parse Serial for valid command.
bool MotorboardCommands::ParseCommand(MotorboardCommands::TMotorboardCommand *Result)
{
  TCommandType CommandType;
  int8_t Motor_Perc;
  uint16_t Duration_Ms;

  TTokenType TokenType;

  if (!PeekNextToken(&TokenType)) return false;

  if (TokenType == Token_Word)
  {
    if (!GetCommandType(&CommandType)) return false;

    Result->Type = CommandType;

    switch (CommandType)
    {
      case Command_LeftMotor:
      case Command_RightMotor:
        if (!GetMotorValue(&Motor_Perc)) return false;
        Result->MotorSpeed = Motor_Perc;
        break;

      case Command_Duration:
        if (!GetDurationValue(&Duration_Ms)) return false;
        Result->Duration_Ms = Duration_Ms;
        break;

      default:
        return false;
    }
  }
  else
  {
    return false;
  }

  return true;
}

// Debug printer.
void MotorboardCommands::DisplayCommand(TMotorboardCommand Command)
{
  if (
    (Command.Type == Command_LeftMotor) ||
    (Command.Type == Command_RightMotor)
  ) {
    Serial.print("(");
    if (Command.Type == Command_LeftMotor)
    {
      Serial.print("Left: ");
    }
    else if (Command.Type == Command_RightMotor)
    {
      Serial.print("Right: ");
    }
    Serial.print(Command.MotorSpeed);
    Serial.print(")");
  }
  else if (Command.Type == Command_Duration)
  {
    Serial.print("(");
    Serial.print("Duration: ");
    Serial.print(Command.Duration_Ms);
    Serial.print(")");
  }
  else
  {
    Serial.print("Unsupported command.");
  }

  Serial.println();
}

/*
  2023-10-14
  2023-11-05
*/
