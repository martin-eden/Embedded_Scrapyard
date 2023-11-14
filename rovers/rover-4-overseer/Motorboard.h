// Interface to motor board.

/*
  Status: works
  Version: 3
  Last mod.: 2023-11-14
*/

#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

class Motorboard
{
  public:
    // Setup motorboard communication channel and test connection.
    bool SetupConnection(uint32_t Baud, uint8_t Receive_Pin, uint8_t Transmit_Pin);

    // Send given M-codes. Returns TRUE if got response.
    bool SendCommand(const char * Commands, uint16_t Timeout_Ms = 5000);

    // Wrappers for ease of use and tracing:

    // Generate command.
    String GenerateCommand(int8_t LeftMotor_Pc, int8_t RightMotor_Pc, uint16_t Duration_Ms);

    // Test connection by sending no-op command to motor board.
    bool TestConnection();

    // Actually spin motors for some time.
    void RunMotorsTest();

    // Exploration. Send neutral command to measure ping.
    uint16_t DetectPing_Ms(uint8_t NumMeasurements = 5);

    // Send command and measure time.
    bool SendCommand_Time_Ms(const char * Command, uint32_t * TimeTaken_Ms);

    // SendCommand with time tracing and debug output.
    bool SendCommand_Trace(const char * Command);

  private:
    HardwareSerial CommentStream = Serial;
    EspSoftwareSerial::UART MotorboardStream;

    // Time to transfer one character. Depends of baud. Set in SetupConnection().
    uint8_t TimePerCharacter_Ms = 0;

    // Maximum time to wait if we think that motorboard is printing help.
    const uint16_t Motorboard_PrintHelpMaxTime_Ms = 4000;

    bool SetupSoftwareSerial(uint32_t Baud, uint8_t Receive_Pin, uint8_t Transmit_Pin);
    bool WaitForReadySignal(uint16_t Timeout_Ms);
};

/*
  2023-11-12
  2023-11-14
*/
