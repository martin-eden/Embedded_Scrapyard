// Interface to motor board.

/*
  Status: finishing base
  Version: 1
  Last mod.: 2023-11-07
*/

#include "Motors.h"

#include <SoftwareSerial.h>

HardwareSerial HardwareSerial_ = Serial;
EspSoftwareSerial::UART SoftwareSerial_;

// ---

bool SetupMotorboardCommunication()
{
  HardwareSerial_.println("Motorboard initialization.");

  const uint32_t Baud = 9600; // 57600; // 115200;
  EspSoftwareSerial::Config Config = SWSERIAL_8N1;
  const uint8_t RX_Pin = D7;
  const uint8_t TX_Pin = D9;

  SoftwareSerial_.begin(Baud, Config, RX_Pin, TX_Pin);

  if (!SoftwareSerial_)
  {
    HardwareSerial_.println("Software serial initialization failed. Stopped.");
    while(1);
  }

  HardwareSerial_.println("Motorboard initialized.");

  return TestConnection();
}

bool TestConnection()
{
  return SendCommand(" ");
}

void HardwareMotorsTest()
{
  HardwareSerial_.println("Motor test.");

  // Send commands to motor board to briefly move motors.
  SendCommand("L 20  R 20  D 100");
  SendCommand("L 40  R 40  D 100");
  SendCommand("L 60  R 60  D 100");
  SendCommand("L 80  R 80  D 100");
  SendCommand("L 99  R 99  D 100");
  SendCommand("L 80  R 80  D 100");
  SendCommand("L 60  R 60  D 100");
  SendCommand("L 40  R 40  D 100");
  SendCommand("L 20  R 20  D 100");
  SendCommand("L  0  R  0  D 100");

  SendCommand("L-20  R-20  D 100");
  SendCommand("L-40  R-40  D 100");
  SendCommand("L-60  R-60  D 100");
  SendCommand("L-80  R-80  D 100");
  SendCommand("L-99  R-99  D 100");
  SendCommand("L-80  R-80  D 100");
  SendCommand("L-60  R-60  D 100");
  SendCommand("L-40  R-40  D 100");
  SendCommand("L-20  R-20  D 100");
  SendCommand("L  0  R  0  D 100");

  HardwareSerial_.println("Motor test done.");
}

uint32_t GetTimePassed_Ms(uint32_t LastTimestamp_MS);

bool SendCommand(const char * Command)
{
  const uint32_t ResponseAwaitTimeout_Ms = 5000;
  bool GotResponse = false;

  // Sending data:
  SoftwareSerial_.write(Command);

  // Waiting for response:
  const uint32_t ResponseWaitStartTime_Ms = millis();
  uint32_t ResponseReceiveTime_Ms = 0;
  while(GetTimePassed_Ms(ResponseWaitStartTime_Ms) < ResponseAwaitTimeout_Ms)
  {
    if (SoftwareSerial_.available())
    {
      GotResponse = true;
      ResponseReceiveTime_Ms = millis();
      break;
    }
  }

  if (GotResponse)
  {
    // Discarding respone. Important thing is that we got feedback.
    while(SoftwareSerial_.available())
    {
       SoftwareSerial_.flush();
    }
    HardwareSerial_.printf("Got response in %d ms.\n", ResponseReceiveTime_Ms);
  }

  return GotResponse;
}

// ---

uint32_t GetTimePassed_Ms(uint32_t LastTimestamp_MS)
{
  return millis() - LastTimestamp_MS;
}
