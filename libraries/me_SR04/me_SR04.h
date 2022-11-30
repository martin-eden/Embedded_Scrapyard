// Ultrasonic distance sensor HC-SR04.

/*
  Version: 4
  Last mod.: 2022-12-01
*/

/*
  Fields

    void Ping();
      Actually ping. Takes time.

  Uses
    none
*/

#pragma once

#include <Arduino.h>

namespace me_SR04
{
  class Sonar
  {
    public:
      Sonar(uint8_t aTriggerPin, uint8_t aEchoPin):
        TriggerPin(aTriggerPin), EchoPin(aEchoPin)
        {};

      uint8_t GetTriggerPin();
      uint8_t GetEchoPin();

      void Ping();

      uint32_t EchoDelayMcr();
      bool NoSignalStart();
      bool NoSignalEnd();

    private:
      uint8_t TriggerPin;
      uint8_t EchoPin;

      bool IsStarted;
      bool IsEnded;
      uint32_t _EchoDelayMcr;

      void Throw();
      void Catch();
  };
}
