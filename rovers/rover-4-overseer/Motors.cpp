// Interface to motor board.

/*
  Status: works
  Version: 2
  Last mod.: 2023-11-09
*/

#include "Motors.h"

#include <SoftwareSerial.h>

HardwareSerial & HardwareSerial_ = Serial;
EspSoftwareSerial::UART SoftwareSerial_;

// ---

// Setup communication channel and test connection.
bool SetupMotorboardCommunication()
{
  bool Result = false;

  HardwareSerial_.print("Motorboard initialization... ");

  const uint32_t Baud = 9600; // 57600; // 115200;
  EspSoftwareSerial::Config Config = SWSERIAL_8N1;
  const uint8_t RX_Pin = D7;
  const uint8_t TX_Pin = D9;

  SoftwareSerial_.begin(Baud, Config, RX_Pin, TX_Pin);

  if (!SoftwareSerial_)
  {
    // Well, this shouldn't happen.
    HardwareSerial_.println("Software serial initialization failed. Stopped.");
    while(1);
  }

  Result = TestConnection();

  if (Result)
    HardwareSerial_.println("yep!");
  else
    HardwareSerial_.println("nah.");

  return Result;
}

// Send dummy command to get feedback.
bool TestConnection()
{
  return SendCommand(" ");
}

// Send commands to motor board to briefly move motors.
void HardwareMotorsTest()
{
  HardwareSerial_.println("Motor test.");

  SendCommand("L 20  R 20  D 10");
  SendCommand("L 40  R 40  D 10");
  SendCommand("L 60  R 60  D 10");
  SendCommand("L 80  R 80  D 10");
  SendCommand("L 99  R 99  D 10");
  SendCommand("L 80  R 80  D 10");
  SendCommand("L 60  R 60  D 10");
  SendCommand("L 40  R 40  D 10");
  SendCommand("L 20  R 20  D 10");
  SendCommand("L  0  R  0  D 10");

  SendCommand("L-20  R-20  D 10");
  SendCommand("L-40  R-40  D 10");
  SendCommand("L-60  R-60  D 10");
  SendCommand("L-80  R-80  D 10");
  SendCommand("L-99  R-99  D 10");
  SendCommand("L-80  R-80  D 10");
  SendCommand("L-60  R-60  D 10");
  SendCommand("L-40  R-40  D 10");
  SendCommand("L-20  R-20  D 10");
  SendCommand("L  0  R  0  D 10");

  HardwareSerial_.println("Motor test done.");
}

uint32_t GetTimePassed_Ms(uint32_t StartTime_Ms, uint32_t EndTime_Ms = 0);

/*
  Core function.

  Send command to motor board and wait for feedback.
*/
bool SendCommand(const char * Command)
{
  const uint32_t ResponseWaitTimeout_Ms = 5000;

  bool GotResponse = false;

  // Discarding any non-read data from motor board:
  while(SoftwareSerial_.available())
  {
     SoftwareSerial_.flush();
     delay(1);
  }

  // Sending data:
  SoftwareSerial_.write(Command);

  // Waiting for response:
  uint32_t ResponseWaitStartTime_Ms = millis();
  while(GetTimePassed_Ms(ResponseWaitStartTime_Ms) < ResponseWaitTimeout_Ms)
  {
    if (SoftwareSerial_.available() == 2)
    {
      char c1 = SoftwareSerial_.read();
      char c2 = SoftwareSerial_.read();

      // Correct response is "G <NewLine>":
      GotResponse = (c1 == 'G') && (c2 == '\n');

      break;
    }
    delay(1);
  }

  // Discarding other response. Important thing is that we got feedback.
  while(SoftwareSerial_.available())
  {
     SoftwareSerial_.flush();
     delay(1);
  }

  return GotResponse;
}

// ---

uint32_t GetTimePassed_Ms(uint32_t StartTime_Ms, uint32_t EndTime_Ms /* = 0 */)
{
  if (EndTime_Ms == 0)
    EndTime_Ms = millis();

  return EndTime_Ms - StartTime_Ms;
}

// SendCommand with time tracing.
bool SendCommand_Trace(const char * Command)
{
  HardwareSerial_.printf(
    "[%lu] Ping_Ms(SendCommand(\"%s\")): ",
    millis(),
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

// Exploration. Sending commands to measure ping.
void FigureOutPingOfBoard()
{

  //* (1)
  SendCommand_Trace("D 1000");
  SendCommand_Trace("D 1000");
  SendCommand_Trace("D 1000");
  // (1) */

  /* (2)
  SendCommand_Trace("D 0");

  SendCommand_Trace("D 1");
  SendCommand_Trace("D 2");
  SendCommand_Trace("D 3");
  SendCommand_Trace("D 5");
  SendCommand_Trace("D 8");
  SendCommand_Trace("D 13");

  SendCommand_Trace("D 10");
  SendCommand_Trace("D 20");
  SendCommand_Trace("D 30");
  SendCommand_Trace("D 50");
  SendCommand_Trace("D 80");
  SendCommand_Trace("D 130");

  SendCommand_Trace("D 100");
  SendCommand_Trace("D 200");
  SendCommand_Trace("D 300");
  SendCommand_Trace("D 500");
  SendCommand_Trace("D 800");
  SendCommand_Trace("D 1300");

  SendCommand_Trace("D 1000");
  SendCommand_Trace("D 2000");
  SendCommand_Trace("D 3000");
  SendCommand_Trace("D 5000");
  SendCommand_Trace("D 8000");
  SendCommand_Trace("D 13000");

  SendCommand_Trace("D 4900");
  SendCommand_Trace("D 5000");
  SendCommand_Trace("D 5100");

  SendCommand_Trace("D 1000");
  SendCommand_Trace("D 1000");
  SendCommand_Trace("D 1000");
  SendCommand_Trace("D 1000");
  SendCommand_Trace("D 1000");
  SendCommand_Trace("D 1000");
  SendCommand_Trace("D 1000");
  // (2) */

  HardwareSerial_.println("Done.");

  /*
    Typical ping on 9600 baud is [143, 153] ms. Let it be 150 ms.

    That means that time gap between messages is that value.

    Continuous control packets should have execution time more than
    this value.
  */
}

/*
  2023-11-07
  2023-11-09
*/
