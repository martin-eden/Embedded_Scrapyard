// Serial() parser for motor board commands.

/*
  Status: stable
  Version: 1
  Last mod.: 2023-10-14
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

// Lexer.
// Note: Reads Serial().
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

// Command names.
const char
  CommandName_LeftMotor = 'L',
  CommandName_RightMotor = 'R',
  CommandName_Duration = 'D';

// Convert command name to command type.
// Note: Reads Serial().
bool GetCommandType(TCommandType *CommandType)
{
  bool IsOk = false;

  if (!Serial.available())
  {
    return IsOk;
  }

  char IncomingChar;
  IncomingChar = Serial.read();

  if (IncomingChar == CommandName_LeftMotor)
  {
    *CommandType = Command_LeftMotor;
    IsOk = true;
  }
  else if (IncomingChar == CommandName_RightMotor)
  {
    *CommandType = Command_RightMotor;
    IsOk = true;
  }
  else if (IncomingChar == CommandName_Duration)
  {
    *CommandType = Command_Duration;
    IsOk = true;
  }

  return IsOk;
}

// Note: Motor value accepted range is [-100, 100].
// Note: Reads Serial().
bool GetMotorValue(int8_t *MotorValue)
{
  bool IsOk = false;

  if (!Serial.available())
  {
    return IsOk;
  }

  LookaheadMode lookaheadMode = SKIP_WHITESPACE;
  int32_t ParseIntValue = Serial.parseInt(lookaheadMode);

  if ((ParseIntValue >= -100) && (ParseIntValue <= 100))
  {
    *MotorValue = ParseIntValue;
    IsOk = true;
  }

  return IsOk;
}

// Note: Duration (ms) accepted range is [0, 5000].
// Note: Reads Serial().
bool GetDurationValue(uint16_t *DurationValue)
{
  bool IsOk = false;

  if (!Serial.available())
  {
    return IsOk;
  }

  LookaheadMode lookaheadMode = SKIP_WHITESPACE;
  int32_t ParseIntValue = Serial.parseInt(lookaheadMode);

  if ((ParseIntValue >= 0) && (ParseIntValue <= 5000))
  {
    *DurationValue = ParseIntValue;
    IsOk = true;
  }

  return IsOk;
}

// Main. Parse Serial() for valid command.
bool MotorboardCommands::ParseCommand(MotorboardCommands::TMotorboardCommand *Result)
{
  bool IsOk = false;

  TCommandType CommandType;
  int8_t MotorValue;
  uint16_t DurationValue;

  TTokenType TokenType;

  IsOk = PeekNextToken(&TokenType);

  if (!IsOk)
  {
    return IsOk;
  }

  if (TokenType == Token_Word)
  {
    IsOk = GetCommandType(&CommandType);
    if (!IsOk)
    {
      return IsOk;
    }

    if (CommandType == Command_LeftMotor)
    {
      IsOk = GetMotorValue(&MotorValue);
      if (!IsOk)
      {
        return IsOk;
      }

      Result->Type = Command_LeftMotor;
      Result->MotorSpeed = MotorValue;
    }
    else if (CommandType == Command_RightMotor)
    {
      IsOk = GetMotorValue(&MotorValue);
      if (!IsOk)
      {
        return IsOk;
      }

      Result->Type = Command_RightMotor;
      Result->MotorSpeed = MotorValue;
    }
    else if (CommandType == Command_Duration)
    {
      IsOk = GetDurationValue(&DurationValue);
      if (!IsOk)
      {
        return IsOk;
      }

      Result->Type = Command_Duration;
      Result->Duration_Ms = DurationValue;
    }
    else
    {
      IsOk = false;
    }
  }

  return IsOk;
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
