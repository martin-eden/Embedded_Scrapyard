#pragma once

#include <Arduino.h>

namespace me_Stat_AverageFilter
{
  class Filter
  {
    public:
      Filter(uint32_t aMaxNumValues) : MaxNumValues(aMaxNumValues) { Reset(); };

      void Reset();
      void Add(float Value);
      float Get();

    private:
      uint32_t MaxNumValues;
      uint32_t NumValues;
      float Average;
  };
}
