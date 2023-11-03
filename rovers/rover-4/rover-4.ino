// Firmware for my Rover v4.

/*
  Status: stable
  Version: 3
  Last mod.: 2023-11-02
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
  1: Motor controller

    Board: Arduino Uno
    Processor: ATmega328P
    Motor board: Deek (stacked)
*/

/*
  Motor board command format.

  Command is two tokens. Command type and command value.

  "L" [-100, 100] -- Left motor. Set specified power and direction.
  "R" [-100, 100] -- Right motor. Set specified power and direction.
  "D" [0, 5000] -- Delay for given time in milliseconds.

  Whitespaces are stripped, so "L 50 R -50 D 1000" == "L50R50D1000".
*/

/*
  Motors are stopped when no command received in some interval of time.
*/

#include <me_DeekMotor.h>
#include <me_Parser_MotorBoard.h>

const uint8_t
  Deek_DirA_Pin = 12,
  Deek_PwmA_Pin = 3,
  Deek_BrakeA_Pin = 9,

  Deek_DirB_Pin = 13,
  Deek_PwmB_Pin = 11,
  Deek_BrakeB_Pin = 8;

const uint32_t
  SerialSpeed = 115200;

const static char
  CommandReferenceText[] PROGMEM =
    "Motor board command format.\n"
    "\n"
    "Command is two tokens. Command type and command value.\n"
    "\n"
    "\"L\" [-100, 100] -- Left motor. Set specified power and direction.\n"
    "\"R\" [-100, 100] -- Right motor. Set specified power and direction.\n"
    "\"D\" [0, 5000] -- Delay for given time in milliseconds.\n"
    "\n"
    "Whitespaces are stripped, so \"L 50 R -50 D 1000\" == \"L50R50D1000\".\n"
    "\n"
    "Motors are stopped when no command received in some interval of time.\n"
    ;

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

using namespace MotorboardCommands; // mostly for TMotorboardCommand

void SetupSerial()
{
  const uint16_t SerialParseIntTimeout_Ms = 100;

  Serial.begin(SerialSpeed);
  Serial.setTimeout(SerialParseIntTimeout_Ms);
}

void PrintSetupGreeting()
{
  Serial.println();
  Serial.println("---------------------");
  Serial.println(" Rover-4 motor board ");
  Serial.println("---------------------");
}

void PrintHelp()
{
  char c;
  for (size_t i = 0; i < strlen_P(CommandReferenceText); ++i)
  {
    c = pgm_read_byte_near(CommandReferenceText + i);
    Serial.print(c);
  }
}

void PrintLoopGreeting()
{
  Serial.println("Rover-4 motor board:  Loop here!");
}

void ExecuteCommand(TMotorboardCommand Command)
{
  // DisplayCommand(Command);

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

void StopMotors()
{
  ExecuteCommand({Command_LeftMotor, 0});
  ExecuteCommand({Command_RightMotor, 0});
  ExecuteCommand({Command_Duration, 100});

  // Serial.println("Motors are stopped.");
}

void ProcessCommand()
{
  static uint32_t LastSucessfullTime_Ms = 0;
  const uint32_t AutoStopTimeout_Ms = 2000;
  static bool MotorsAreStopped = false;
  uint32_t TimePassed_Ms;

  TMotorboardCommand Command;
  bool IsOk;

  IsOk = ParseCommand(&Command);

  if (IsOk)
  {
    // Serial.println("Got command.");
    // DisplayCommand(Command);

    ExecuteCommand(Command);

    LastSucessfullTime_Ms = millis();

    MotorsAreStopped = false;
  }

  TimePassed_Ms = millis() - LastSucessfullTime_Ms;

  if ((TimePassed_Ms < AutoStopTimeout_Ms) && MotorsAreStopped)
  {
    MotorsAreStopped = false;
  }

  if ((TimePassed_Ms >= AutoStopTimeout_Ms) && !MotorsAreStopped)
  {
    // Serial.println("Auto-stopping motors.");

    StopMotors();

    MotorsAreStopped = true;
  }
}

void setup()
{
  SetupSerial();

  PrintSetupGreeting();
  PrintHelp();
}

void loop()
{
  // PrintLoopGreeting();

  ProcessCommand();

  delay(TickTime_Ms);
}

/*
  2023-10-07
  2023-10-08
  2023-10-11
  2023-10-14
  2023-11-02
*/
