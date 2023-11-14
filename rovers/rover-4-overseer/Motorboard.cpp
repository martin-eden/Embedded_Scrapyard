// Interface to motor board.

/*
  Status: works
  Version: 7
  Last mod.: 2023-11-14
*/

#include "Motorboard.h"

// -- internal helpers

uint32_t GetTimePassed_Ms(uint32_t StartTime_Ms, uint32_t EndTime_Ms = 0)
{
  if (EndTime_Ms == 0)
    EndTime_Ms = millis();

  return EndTime_Ms - StartTime_Ms;
}

// -- public

using namespace Motorboard;

// Setup communication channel and test connection.
bool MotorboardChannel::Setup(
  uint32_t Baud,
  uint8_t Receive_Pin,
  uint8_t Transmit_Pin
)
{
  bool IsConnected = false;

  CommentStream->print("Motorboard initialization... ");

  if (!SetupChannel(Baud, Receive_Pin, Transmit_Pin))
  {
    // This never happened in my experience.
    CommentStream->println("Software serial initialization failed.");
    return false;
  }

  TimePerCharacter_Ms = 1000 / (Baud / 10) + 1;

  IsConnected = Test();

  if (!IsConnected)
  {
    uint16_t MotorboardSetupGracePeriod_Ms = 3000;
    delay(MotorboardSetupGracePeriod_Ms);
    IsConnected = Test();
  }

  if (IsConnected)
    CommentStream->println("yep.");
  else
    CommentStream->println("nah!");

  return IsConnected;
}

/*
  Core function.

  Send command to motor board and wait for feedback.

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

    Command is limited to chunk size. I expect chunk size is less than
    100 bytes. So theoretical limit is how many time may be spent on
    processing 100 byte string. "D5000" takes 5 second per 5 bytes.
    So 100 seconds.
*/
bool MotorboardChannel::Send(const char * Command, uint16_t Timeout_Ms)
{
  if (MotorboardStream.available())
  {
    /*
      Motorboard is sending something to us. Thats not typical.

      Probably it's startup help text. Wait for ready signal.
      It should appear after help text is printed.
    */

    if (!WaitForReadySignal(ReadingMaxTime_Ms))
      return false;
  }

  MotorboardStream.write(Command);

  return WaitForReadySignal(Timeout_Ms);
}

// Send dummy command to get feedback.
bool MotorboardChannel::Test()
{
  uint16_t TestCommandTimeout_Ms = 3 * TimePerCharacter_Ms + 10;

  return Send(" ", TestCommandTimeout_Ms);
}

// -- private

bool MotorboardChannel::SetupChannel(
  uint32_t Baud,
  uint8_t Receive_Pin,
  uint8_t Transmit_Pin
)
{
  const EspSoftwareSerial::Config ByteEncoding = SWSERIAL_8N1;

  MotorboardStream.begin(
    Baud,
    ByteEncoding,
    Receive_Pin,
    Transmit_Pin
  );

  return (bool) MotorboardStream;
}

bool MotorboardChannel::WaitForReadySignal(uint16_t Timeout_Ms)
{
  /*
    Waiting for response.

    We ignoring all side output from board and waiting for "\nG\n"
    and empty stream as a signal that board is ready for further
    commands.
  */
  char Chars[3] = "";

  uint32_t StartTime_Ms = millis();

  while (GetTimePassed_Ms(StartTime_Ms) < Timeout_Ms)
  {
    if (MotorboardStream.available())
    {
      Chars[2] = Chars[1];
      Chars[1] = Chars[0];
      Chars[0] = MotorboardStream.read();

      // Correct response is "\nG\n":
      if ((Chars[2] == '\n') && (Chars[1] == 'G') && (Chars[0] == '\n'))
      {
        delay(TimePerCharacter_Ms);

        if (!MotorboardStream.available())
          return true;
      }
    }

    delay(TimePerCharacter_Ms);
  }

  return false;
}

// -- Wrappers and side stuff

// Profiling. Send command and measure time.
bool Motorboard::Send_Time_Ms(
  MotorboardChannel& MotorboardChannel_,
  const char * Command,
  uint32_t * TimeTaken_Ms
)
{
  uint32_t StartTime_Ms = millis();

  bool SendResult = MotorboardChannel_.Send(Command);

  *TimeTaken_Ms = GetTimePassed_Ms(StartTime_Ms);

  return SendResult;
}

// Tracing. Send command amd print measured duration (ms).
bool Motorboard::Send_Trace(
  MotorboardChannel& MotorboardChannel_,
  const char * Command
)
{
  CommentStream->printf("Send_Time_Ms(\"%s\"): ", Command);

  uint32_t TimeTaken_Ms;

  bool SendResult =
    Send_Time_Ms(MotorboardChannel_, Command, &TimeTaken_Ms);

  CommentStream->printf("%u\n", TimeTaken_Ms);

  return SendResult;
}

// Exploration. Send commands to measure ping.
uint16_t Motorboard::MeasurePing_Ms(
  MotorboardChannel& MotorboardChannel_,
  uint8_t NumMeasurements
)
{
  // We don't even wanna talk about zero measurements case:
  if (NumMeasurements == 0)
    return 0;

  const char Command[] = " ";

  uint32_t TimePassed_Ms = 0;
  uint8_t NumMeasurementsDone;

  for (NumMeasurementsDone = 0; NumMeasurementsDone < NumMeasurements; ++NumMeasurementsDone)
  {
    uint32_t Send_Duration_Ms;
    if (!Send_Time_Ms(MotorboardChannel_, Command, &Send_Duration_Ms))
      break;
    TimePassed_Ms += Send_Duration_Ms;
  }

  return TimePassed_Ms / NumMeasurementsDone;
}

/*
  Generate command in general format.

  The longer the command, the more time it will take to transfer.

  Spaces in format string are for readability. But the last non-digit
  symbol ensures that motorboard will not spend additional time waiting
  digits for continuation of number.
*/
String Motorboard::GenerateCommand(
  int8_t LeftMotor_Pc,
  int8_t RightMotor_Pc,
  uint16_t Duration_Ms
)
{
  uint8_t MaxCommandSize = 32;
  char Command[MaxCommandSize];

  snprintf(
    Command,
    MaxCommandSize,
    "L%d R%d D%d;",
    LeftMotor_Pc,
    RightMotor_Pc,
    Duration_Ms
  );

  return String(Command);
}

/*
  Send commands to motor board to briefly move motors.

  Originally it was linear progression [0, 100, 0, -100, 0].
  But it became too boring when I tested and debugged code.
  So now its sine sweep. Motor power is sin([0, 360]).

  Nonlinear acceleration as derivative of sine is not constant.
*/
void Motorboard::RunMotorsTest(MotorboardChannel & MotorboardChannel_)
{
  CommentStream->printf("Motors test.. ");

  /*
    Ideal test duration.

    Actual test time will be longer as sending commands will take
    additional time (~7 ms per command for 57600 baud).
  */
  const uint16_t TestDuration_Ms = 800;
  const uint16_t NumCommands = 12;

  const uint16_t CommandDuration_Ms = TestDuration_Ms / NumCommands;

  const uint16_t NumAnglesInCircle = 360;
  const uint16_t AngleIncerement = NumAnglesInCircle / NumCommands;
  const uint8_t Amplitude = 100;

  uint16_t Angle = 0;
  while (1)
  {
    if (Angle > NumAnglesInCircle)
      Angle = NumAnglesInCircle;

    float Angle_Rad = (float) Angle / NumAnglesInCircle * (2 * M_PI);
    int8_t MotorPower_Pc = Amplitude * sin(Angle_Rad);

    MotorboardChannel_.Send(
      GenerateCommand(MotorPower_Pc, MotorPower_Pc, CommandDuration_Ms).c_str()
    );

    if (Angle == NumAnglesInCircle)
      break;

    Angle += AngleIncerement;
  }

  CommentStream->printf("done.\n");
}

// ---

/*
  2023-11-07
  2023-11-09
  2023-11-11
  2023-11-12
  2023-11-14
*/
