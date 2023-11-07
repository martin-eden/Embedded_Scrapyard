// Interface to motor board.

/*
  Status: extending and reworking
  Version: 1
  Last mod.: 2023-11-07
*/

#pragma once

#include <Arduino.h>

// Setup motorboard communication channel and test connection.
bool SetupMotorboardCommunication();

// Test connection by sending no-op command to motor board.
bool TestConnection();

// Send given M-codes. Returns TRUE if got response.
bool SendCommand(const char * Command);

// Actually spin motors for some time.
void HardwareMotorsTest();
