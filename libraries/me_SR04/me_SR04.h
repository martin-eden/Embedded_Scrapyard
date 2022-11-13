// Ultrasonic distance sensor HC-SR04

/*
  Status: stable
  Version: 1.2
  Last mod.: 2022-11-13
*/

/*
  Usage

    Sensor.Ping()
    if (Sensor.RequestStatus == Success)
      Delay = Sensor.EchoDelayMcr
      Distance = ConvertToDistance(Delay)

  Fields overview

    void Ping()

      Send signal and wait for return echo.

      Has built-in timeout if no echo is received.

      Updates <RequestStatus> and <EchoDelayMcr>.

    Status RequestStatus

      Possible values are

        Unknown
        Success
        NoSignalStart
        NoSignalEnd

    uint32_t EchoDelayMcr

      Delay in microseconds between pulse and echo.

      Use it to calculate distance.
*/

#pragma once

#include <Arduino.h>

namespace me_SR04
{
  enum class Status
  {
    Unknown,
    Success,
    NoSignalStart,
    NoSignalEnd
  };

  class SR04
  {
    public:
      Status RequestStatus;
      uint32_t EchoDelayMcr;

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
