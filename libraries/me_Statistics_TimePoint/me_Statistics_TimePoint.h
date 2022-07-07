#pragma once

#include <Arduino.h>

namespace me_Statistics_TimePoint
{
  class TimePoint
  {
    public:
      TimePoint(float aValue, uint32_t aTime) : Value(aValue), Time(aTime) {};
      float Value;
      uint32_t Time;
    private:
  };
}
