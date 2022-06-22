// Ultrasonic distance sensor HC-SR04

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

      SR04(uint8_t aTriggerPin, uint8_t aEchoPin) : TriggerPin(aTriggerPin), EchoPin(aEchoPin) {};
      void Request();

    private:
      uint8_t TriggerPin;
      uint8_t EchoPin;
  };
}
