// Ultrasonic distance sensor HC-SR04

/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-11-01
*/

/*
  Wiring

    Sensor pin name | Program pin name
    ----------------+-----------------
    Trig            | TriggerPin
    Echo            | EchoPin

    Trig and Echo can be connected to one physical pin.

  Init

    SR04(uint8_t aTriggerPin, uint8_t aEchoPin)

  Usage

    void Ping()

      Send signal and wait for return echo.

      Has built-in timeout if no echo is received.

      Updates <RequestStatus> and <EchoDelayMcr>.

    ReadStatus RequestStatus

      If <RequestStatus> is <Success> then use <EchoDelayMcr>.

      Possible values are

        Unknown
        Success
        NoSignalStart
        NoSignalEnd

    uint32_t EchoDelayMcr

      Delay in microseconds between pulse and echo. Use it to calculate
      distance.
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
