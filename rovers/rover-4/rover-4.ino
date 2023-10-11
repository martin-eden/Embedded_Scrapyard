// Firmware for my Rover v4.

/*
  Status: stable
  Version: 2
  Last mod.: 2023-10-11
*/

/*
  This is code for Arduino Uno with stacked Deek motor shield.
*/

/*
  I've got troubles using <ESP8266TimerInterrupt.h> and Deek motor
  shield stacked top on board. When I start using timer interrupt,
  D2 goes high and D2 is Motor A PWM pin.

  As a workaround, NEW Arduino Uno BOARD added just for Deek motor
  shield. It is connected to some master board over UART.
*/

/*
  1: Motor shield

    Board: Arduino Uno
    Processor: ATmega328P
    Motor board: Deek (stacked)

  2: Overseer

    Board: WeMos D1 ESP WROOM WiFi UNO
    Processor: ESP 8266
    Accelerometer: MPU6050 (I2C)
*/

/*
  Motor board command format.

  Command is two tokens. Command type and command value.

  "L" [-100, 100] -- Left motor. Set specified power and direction.
  "R" [-100, 100] -- Right motor. Set specified power and direction.
  "D" [0, 5000] -- Delay for given value in milliseconds.

  Whitespaces are stripped, so "L 50 R -50 D 1000" == "L50R50D1000".
*/

#include <Stream.h>

#include <me_DeekMotor.h>
// #include <me_TwoDcMotorsDirector.h>

const uint8_t
  Deek_DirA_Pin = 12,
  Deek_PwmA_Pin = 3,
  Deek_BrakeA_Pin = 9,

  Deek_DirB_Pin = 13,
  Deek_PwmB_Pin = 11,
  Deek_BrakeB_Pin = 8;

const uint32_t
  SerialSpeed = 9600;

const uint32_t
  TickTime_Ms = 50;

const TDeekMotorPins
  LeftMotorPins =
    {
      Direction_Pin: Deek_DirA_Pin,
      Pwm_Pin: Deek_PwmA_Pin,
      Brake_Pin: Deek_BrakeA_Pin
    },
  RightMotorPins =
    {
      Direction_Pin: Deek_DirB_Pin,
      Pwm_Pin: Deek_PwmB_Pin,
      Brake_Pin: Deek_BrakeB_Pin
    };

DeekMotor LeftMotor(LeftMotorPins);
DeekMotor RightMotor(RightMotorPins);

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

enum TTokenType
{
  Token_Word,
  Token_Number
};

const char
  CommandName_LeftMotor = 'L',
  CommandName_RightMotor = 'R',
  CommandName_Duration = 'D';

void StopMotors()
{
  LeftMotor.SetSpeed(0);
  RightMotor.SetSpeed(0);
}

void DoMotorsTest()
{
  uint8_t acceleration = 2;
  for (int16_t angle_deg = 0; angle_deg < 360; angle_deg = angle_deg + acceleration)
  {
    int8_t Speed;

    float MagnifiedSine = sin(DEG_TO_RAD * angle_deg) * 100;

    Speed = MagnifiedSine;

    LeftMotor.SetSpeed(Speed);
    RightMotor.SetSpeed(Speed);

    delay(20);
  }

  StopMotors();
}

void PrintSetupGreeting()
{
  Serial.println();
  Serial.println("---------------------");
  Serial.println("Rover-4: Hello there!");
  Serial.println("---------------------");
}

void PrintLoopGreeting()
{
  Serial.println("-------------------");
  Serial.println("Rover-4: Loop here!");
  Serial.println("-------------------");
}

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

bool ParseCommand(TMotorboardCommand *Result)
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

void ExecuteCommand(TMotorboardCommand Command)
{
  switch (Command.Type)
  {
    case Command_LeftMotor:
      LeftMotor.SetSpeed(Command.MotorSpeed);
      break;

    case Command_RightMotor:
      RightMotor.SetSpeed(Command.MotorSpeed);
      break;

    case Command_Duration:
      delay(Command.Duration_Ms);
      break;
  }
}

void DisplayCommand(TMotorboardCommand Command)
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
    Serial.print("Unrecognized command.");
  }

  Serial.println();
}

void ProcessCommand()
{
  static uint32_t LastSucessfullTime = 0;
  const uint32_t AutoStopTimeout_Ms = 20;
  static bool MotorsAreStopped = false;

  TMotorboardCommand Command;
  bool IsOk;

  IsOk = ParseCommand(&Command);
  if (IsOk)
  {
    DisplayCommand(Command);
    ExecuteCommand(Command);
    LastSucessfullTime = millis();
    MotorsAreStopped = false;
  }
  else
  {
    if (!MotorsAreStopped)
    {
      // Serial.println(millis() - LastSucessfullTime);
    }

    if (!MotorsAreStopped && (millis() - LastSucessfullTime) >= AutoStopTimeout_Ms)
    {
      StopMotors();
      // Serial.println("Motors are stopped.");
      MotorsAreStopped = true;
    }
  }
}

void SetupSerial()
{
  const uint16_t SerialParseIntTimeout_Ms = 100;

  Serial.begin(SerialSpeed);
  Serial.setTimeout(SerialParseIntTimeout_Ms);
}

void setup()
{
  SetupSerial();

  PrintSetupGreeting();

  DoMotorsTest();
}

void loop()
{
  // PrintLoopGreeting();

  uint32_t StartTime = millis();

  ProcessCommand();

  uint32_t CurrentTime = millis();
  uint32_t TimePassed = CurrentTime - StartTime;
  if (TimePassed < TickTime_Ms)
  {
    delay(TickTime_Ms - TimePassed);
  }
}

/*
  2023-10-07
  2023-10-08
  2023-10-11
*/
