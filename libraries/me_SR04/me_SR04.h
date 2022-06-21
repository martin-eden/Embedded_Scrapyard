// Ultrasonic distance sensor HC-SR04

#pragma once

#include <Arduino.h>

namespace me_SR04
{
  enum class ReadStatus
  {
    Unknown,
    Success,
    Error
  };

  class SR04
  {
    public:
      uint32_t EchoDelayMcr;
      uint32_t EchoDurationMcr;
      ReadStatus RequestStatus;

      SR04(uint8_t aTriggerPin, uint8_t aEchoPin) : TriggerPin(aTriggerPin), EchoPin(aEchoPin) {};
      void Request();

    private:
      uint8_t TriggerPin;
      uint8_t EchoPin;
  };
}
