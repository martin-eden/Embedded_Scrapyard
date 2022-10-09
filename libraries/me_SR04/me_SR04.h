// Ultrasonic distance sensor HC-SR04

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-10-09
*/

/*
  Pins:
    "Trig" - TriggerPin - output pin to emit ping
    "Echo" - EchoPin - input pin to receive echo

  Hints:
    * You can wire "Trig" and "Echo" to one pin.

  Init:
    TriggerPin, EchoPin

  Usage:
    Ping()

  Output:
    EchoDelayMcr - uint32_t
    RequestStatus - ReadStatus:
      (Unknown, Success, NoSignalStart, NoSignalEnd)
*/

#pragma once

#include <Arduino.h>

namespace me_SR04
{
  enum class ReadStatus
  {
    Unknown,
    Success,
    NoSignalStart,
    NoSignalEnd
  };

  class SR04
  {
    public:
      uint32_t EchoDelayMcr;
      ReadStatus RequestStatus;

      SR04(uint8_t aTriggerPin, uint8_t aEchoPin) :
        TriggerPin(aTriggerPin), EchoPin(aEchoPin) {};
      void Ping();

    private:
      uint8_t TriggerPin;
      uint8_t EchoPin;

      void Throw();
      void Catch(bool *IsStarted, bool *IsEnded, uint32_t *Duration);
  };
}
