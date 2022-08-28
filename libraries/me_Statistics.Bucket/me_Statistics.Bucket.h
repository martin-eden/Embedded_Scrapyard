#pragma once

#include <Arduino.h>

namespace me_Statistics_Bucket
{
  class Bucket
  {
    public:
      Bucket(float Span = 1.0);
      void Reset(float Span = 1.0);
      bool Add(float Value);
      float GetCurrentSpan();

    private:
      float Span;
      float MinValue;
      float MaxValue;
  };
}
