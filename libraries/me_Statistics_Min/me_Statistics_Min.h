#pragma once

#include <Arduino.h>

#include <me_Statistics_TimePoint.h>

namespace me_Statistics_Min
{
  using namespace me_Statistics_TimePoint;

  class MinValue
  {
    public:
      MinValue(TimePoint aTimePoint) : timePoint(aTimePoint), isUpdated(false) {};

      void Add(TimePoint aTimePoint);
      TimePoint Get();
      bool ValueHasChanged();

    private:
      TimePoint timePoint;
      bool isUpdated;
  };
}
