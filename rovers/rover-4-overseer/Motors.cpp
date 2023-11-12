// Interface to motor board.

/*
  Status: works
  Version: 5
  Last mod.: 2023-11-11
*/

#include "Motors.h"

#include <SoftwareSerial.h>

HardwareSerial & HardwareSerial_ = Serial;
EspSoftwareSerial::UART SoftwareSerial_;

// ---

// Setup communication channel and test connection.
bool SetupMotorboardCommunication(uint32_t Baud, uint8_t Receive_Pin, uint8_t Transmit_Pin)
{
  bool IsConnected = false;

  HardwareSerial_.print("Motorboard initialization... ");

  EspSoftwareSerial::Config Config = SWSERIAL_8N1;

  SoftwareSerial_.begin(Baud, Config, Receive_Pin, Transmit_Pin);

  if(!SoftwareSerial_)
  {
    // Well, this shouldn't happen.
    HardwareSerial_.println("Software serial initialization failed. Stopped.");
    while(1);
  }

  IsConnected = TestConnection();

  if(IsConnected)
    HardwareSerial_.println("yep.");
  else
    HardwareSerial_.println("nah!");

  if(IsConnected)
  {
    HardwareSerial_.print("Measuring motorboard ping: ");
    uint16_t PingValue_Ms = DetectPing_Ms();
    HardwareSerial_.printf("%d ms\n", PingValue_Ms);
  }

  return IsConnected;
}

uint32_t GetTimePassed_Ms(uint32_t StartTime_Ms, uint32_t EndTime_Ms = 0);

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

    We ignoring all side output from board and waiting for "\nG\n"
    and empty stream as a signal that board is ready for further
    commands.
  */
  char Chars[3] = "";

  uint32_t ResponseWaitStartTime_Ms = millis();

  while(GetTimePassed_Ms(ResponseWaitStartTime_Ms) < ResponseWaitTimeout_Ms)
  {
    if(SoftwareSerial_.available())
    {
      Chars[2] = Chars[1];
      Chars[1] = Chars[0];
      Chars[0] = SoftwareSerial_.read();

      // Correct response is "G <NewLine>":
      if((Chars[2] == '\n') && (Chars[1] == 'G') && (Chars[0] == '\n'))
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

// ---

uint32_t GetTimePassed_Ms(uint32_t StartTime_Ms, uint32_t EndTime_Ms /* = 0 */)
{
  if (EndTime_Ms == 0)
    EndTime_Ms = millis();

  return EndTime_Ms - StartTime_Ms;
}

// Send command and measure time.
bool SendCommand_Time_Ms(const char * Command, uint32_t * TimeTaken_Ms)
{
  uint32_t StartTime_Ms;
  StartTime_Ms = millis();

  bool SendCommandResult;
  SendCommandResult = SendCommand(Command);

  uint32_t EndTime_Ms;
  EndTime_Ms = millis();

  *TimeTaken_Ms = GetTimePassed_Ms(StartTime_Ms, EndTime_Ms);

  return SendCommandResult;
}

// SendCommand with time tracing and debug output.
bool SendCommand_Trace(const char * Command)
{
  HardwareSerial_.printf("SendCommand_Time_Ms(\"%s\"): ", Command);

  uint32_t TimeTaken_Ms = 0;

  bool SendCommandResult = SendCommand_Time_Ms(Command, &TimeTaken_Ms);

  HardwareSerial_.printf("%u\n", TimeTaken_Ms);

  return SendCommandResult;
}

// Send dummy command to get feedback.
bool TestConnection()
{
  return SendCommand(" ");
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

// Exploration. Sending commands to measure ping.
uint16_t DetectPing_Ms(uint8_t NumMeasurements)
{
  char Command[] = " ";

  uint32_t TimePassed_Ms = 0;
  for(uint8_t i = 0; i < NumMeasurements; ++i)
  {
    uint32_t SendCommand_Duration_Ms = 0;

    bool IsSent = SendCommand_Time_Ms(Command, &SendCommand_Duration_Ms);

    if (!IsSent)
      break;

    TimePassed_Ms += SendCommand_Duration_Ms;
  }

  // Average test execution time:
  uint16_t AverageTestTime_Ms = TimePassed_Ms / NumMeasurements;

  return AverageTestTime_Ms;
}

// Send commands to motor board to briefly move motors.
void HardwareMotorsTest()
{
  HardwareSerial_.print("Motors test.. ");

  const uint16_t TestDuration_Ms = 500;
  const uint8_t PowerIncrement_Pc = 25;

  const uint16_t NumCommands = ((100 / PowerIncrement_Pc) + 1) * 4;

  uint16_t PhaseDuration_Ms = TestDuration_Ms / NumCommands;

  int8_t MotorPower_Pc;

  for(MotorPower_Pc = 0; MotorPower_Pc <= 100; MotorPower_Pc += 20)
  {
    SendCommand(
      GenerateCommand(
        MotorPower_Pc,
        MotorPower_Pc,
        PhaseDuration_Ms
      ).c_str()
    );
  }

  for(MotorPower_Pc = 100; MotorPower_Pc >= -100; MotorPower_Pc -= 20)
  {
    SendCommand(
      GenerateCommand(
        MotorPower_Pc,
        MotorPower_Pc,
        PhaseDuration_Ms
      ).c_str()
    );
  }

  for(MotorPower_Pc = -100; MotorPower_Pc <= 0; MotorPower_Pc += 20)
  {
    SendCommand(
      GenerateCommand(
        MotorPower_Pc,
        MotorPower_Pc,
        PhaseDuration_Ms
      ).c_str()
    );
  }

  HardwareSerial_.println("done.");
}

// ---

/*
  Typical ping on 9600 baud is 20 ms.

  10ms is polling interval in motor board. So 5ms average response time.

  Typical command should take like 17 ms. Plus 2 ms to transmit response.

  Should be around 24 ms according to my estimations but actual 20 ms
  is good enough.
*/

/*
  2023-11-07
  2023-11-09
  2023-11-11
*/
