// Interface to motor board.

/*
  Status: works
  Version: 4
  Last mod.: 2023-11-14
*/

#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

namespace Motorboard
{
  constexpr HardwareSerial *CommentStream = &Serial;

  // -- Core class:
  class MotorboardChannel
  {
    public:
      MotorboardChannel() {};

      // Setup motorboard communication channel and test connection.
      bool Setup(uint32_t Baud, uint8_t Receive_Pin, uint8_t Transmit_Pin);

      // Send given M-codes. Returns TRUE if got response.
      bool Send(const char * Commands, uint16_t Timeout_Ms = 5000);

      // Wrappers for ease of use and tracing:

      // Test connection by sending no-op command to motor board.
      bool Test();

    private:
      EspSoftwareSerial::UART MotorboardStream;

      // Time to transfer one character. Depends of baud. Set in SetupConnection().
      uint8_t TimePerCharacter_Ms = 0;

      // Maximum time to wait if we think that motorboard is printing help.
      static const uint16_t ReadingMaxTime_Ms = 4000;

      bool SetupChannel(uint32_t Baud, uint8_t Receive_Pin, uint8_t Transmit_Pin);
      bool WaitForReadySignal(uint16_t Timeout_Ms);
  };

  // -- Some handy extensions:

  // Send command and measure time.
  bool Send_Time_Ms(
    MotorboardChannel& MotorboardChannel_,
    const char * Command,
    uint32_t * TimeTaken_Ms
  );

  // SendCommand with time tracing and debug output.
  bool Send_Trace(
    MotorboardChannel& MotorboardChannel_,
    const char * Command
  );

  // Exploration. Send neutral command to measure ping.
  uint16_t MeasurePing_Ms(
    MotorboardChannel & MotorboardChannel_,
    uint8_t NumMeasurements = 5
  );

  // Generate command.
  String GenerateCommand(
    int8_t LeftMotor_Pc,
    int8_t RightMotor_Pc,
    uint16_t Duration_Ms
  );

  // Actually spin motors for some time.
  void RunMotorsTest(MotorboardChannel & MotorboardChannel_);
}

/*
  2023-11-12
  2023-11-14
*/
