// Interface to motor board.

/*
  Status: works
  Version: 3
  Last mod.: 2023-11-09
*/

#include "Motors.h"

#include <SoftwareSerial.h>

HardwareSerial & HardwareSerial_ = Serial;
EspSoftwareSerial::UART SoftwareSerial_;

// ---

// Setup communication channel and test connection.
bool SetupMotorboardCommunication(uint32_t Baud, uint8_t Receive_Pin, uint8_t Transmit_Pin)
{
  bool Result = false;

  HardwareSerial_.print("Motorboard initialization... ");

  EspSoftwareSerial::Config Config = SWSERIAL_8N1;

  SoftwareSerial_.begin(Baud, Config, Receive_Pin, Transmit_Pin);

  if (!SoftwareSerial_)
  {
    // Well, this shouldn't happen.
    HardwareSerial_.println("Software serial initialization failed. Stopped.");
    while(1);
  }

  Result = TestConnection();

  if (Result)
    HardwareSerial_.println("yep.");
  else
    HardwareSerial_.println("nah!");

  HardwareSerial_.print("Measuring motorboard ping: ");

  uint16_t PingValue_Ms = DetectPing_Ms();

  HardwareSerial_.printf("%d ms\n", PingValue_Ms);

  return Result;
}

// Send dummy command to get feedback.
bool TestConnection()
{
  return SendCommand(" ");
}

// Send commands to motor board to briefly move motors.

bool SendCommand_Trace(const char * Command);
String GenerateCommand(int8_t LeftMotor_Pc, int8_t RightMotor_Pc, uint16_t Duration_Ms);

void HardwareMotorsTest()
{
  HardwareSerial_.print("Motors test.. ");

  const uint16_t TestDuration_Ms = 500;
  const uint8_t PowerIncrement_Pc = 25;

  const uint16_t NumCommands = ((100 / PowerIncrement_Pc) + 1) * 4;

  uint16_t PhaseDuration_Ms = TestDuration_Ms / NumCommands;

  int8_t MotorPower_Pc;

  for (MotorPower_Pc = 0; MotorPower_Pc <= 100; MotorPower_Pc += 20)
  {
    SendCommand_Trace(
      GenerateCommand(
        MotorPower_Pc,
        MotorPower_Pc,
        PhaseDuration_Ms
      ).c_str()
    );
  }

  for (MotorPower_Pc = 100; MotorPower_Pc >= -100; MotorPower_Pc -= 20)
  {
    SendCommand_Trace(
      GenerateCommand(
        MotorPower_Pc,
        MotorPower_Pc,
        PhaseDuration_Ms
      ).c_str()
    );
  }

  for (MotorPower_Pc = -100; MotorPower_Pc <= 0; MotorPower_Pc += 20)
  {
    SendCommand_Trace(
      GenerateCommand(
        MotorPower_Pc,
        MotorPower_Pc,
        PhaseDuration_Ms
      ).c_str()
    );
  }

  HardwareSerial_.println("done.");
}

uint32_t GetTimePassed_Ms(uint32_t StartTime_Ms, uint32_t EndTime_Ms = 0)
{
  if (EndTime_Ms == 0)
    EndTime_Ms = millis();

  return EndTime_Ms - StartTime_Ms;
}

// SendCommand with time tracing.
bool SendCommand_Trace(const char * Command)
{
  HardwareSerial_.printf(
    "Ping_Ms(\"%s\"): ",
    Command
  );

  uint32_t StartTime_Ms;
  StartTime_Ms = millis();

  bool SendCommandResult;
  SendCommandResult = SendCommand(Command);

  uint32_t EndTime_Ms;
  EndTime_Ms = millis();

  HardwareSerial_.printf(
    "%u\n",
    GetTimePassed_Ms(StartTime_Ms, EndTime_Ms)
  );

  return SendCommandResult;
}

String GenerateCommand(int8_t LeftMotor_Pc, int8_t RightMotor_Pc, uint16_t Duration_Ms)
{
  uint8_t MaxCommandSize = 32;
  char Command[MaxCommandSize];

  snprintf(
    Command,
    MaxCommandSize,
    "L %d R %d D %d ",
    LeftMotor_Pc,
    RightMotor_Pc,
    Duration_Ms
  );

  return String(Command);
}

// ---

/*
  Core function.

  Send command to motor board and wait for feedback.
*/
bool SendCommand(const char * Commands)
{
  /*
    Response wait timeout.

    As we do not parse commands, we dont know how they will take to
    execute. We just sending them and waiting for feedback.

    But if connection to motorboard is dropped we will never receive
    feedback. In this case we are stopping listening and exiting by
    timeout.

    So here is tradeoff between maximum command execution time and
    time wasted when connection was dropped.

    Protocol sets high limit for duration phase 5s but we can have
    more commands in that string.
  */
  const uint32_t ResponseWaitTimeout_Ms = 5000;

  // Discarding any non-read data from motor board:
  while(SoftwareSerial_.available())
  {
     SoftwareSerial_.flush();
     delay(10);
  }

  // Sending data:
  SoftwareSerial_.write(Commands);

  /*
    Waiting for response.

    We ignoring all side output from board and waiting for "G\n"
    and empty stream as a signal that board is redy for further
    commands.
  */
  char
    CurChar = '\0',
    PrevChar = '\0';

  uint32_t ResponseWaitStartTime_Ms = millis();

  while (GetTimePassed_Ms(ResponseWaitStartTime_Ms) < ResponseWaitTimeout_Ms)
  {
    if (SoftwareSerial_.available())
    {
      PrevChar = CurChar;
      CurChar = SoftwareSerial_.read();

      // Correct response is "G <NewLine>":
      if ((PrevChar == 'G') && (CurChar == '\n'))
      {
        delay(1);

        if (!SoftwareSerial_.available())
          return true;
      }
    }

    delay(1);
  }

  return false;
}

// Exploration. Sending commands to measure ping.
uint16_t DetectPing_Ms(uint16_t TotalTestDuration_Ms, uint8_t NumMeasurements)
{
  const uint16_t MeasurementDuration_Ms = TotalTestDuration_Ms / NumMeasurements;

  String Command = GenerateCommand(0, 0, MeasurementDuration_Ms);

  uint32_t TimePassed_Ms = 0;
  for (uint8_t i = 0; i < NumMeasurements; ++i)
  {
    uint32_t StartTime_Ms = millis();
    SendCommand_Trace(Command.c_str());
    TimePassed_Ms += GetTimePassed_Ms(StartTime_Ms);
  }

  uint16_t Result;

  // Average test execution time:
  uint16_t AverageTestTime_Ms = TimePassed_Ms / NumMeasurements;

  /*
    If board is not connected, SendCommand() exits by timeout.

    In this case time passed for measurement can be less than expected.
  */
  if (AverageTestTime_Ms > MeasurementDuration_Ms)
  {
    Result = AverageTestTime_Ms - MeasurementDuration_Ms;
  }
  else
  {
    Result = AverageTestTime_Ms;
  }

  return Result;
}

// ---

/*
  Typical ping on 9600 baud is 20 ms.

  10ms is polling interval in motor board. So 5ms average response time.

  Typical command should take like 17 ms. Plus 2 ms to transmit response.

  Should be round 24 ms according to my estimations but actual 20 ms
  is good enough.
*/

/*
  2023-11-07
  2023-11-09
*/
