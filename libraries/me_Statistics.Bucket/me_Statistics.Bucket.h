// Magnetic bucket

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-10-09
*/

/*
  Concept:

    Some sort of basket, bin or bucket that extends to catch the ball
    you throw. It extends to a certain limit. So not any ball can be
    caught.

  Original usage:

    I've used it to partition values from SR04 ultrasonic sensor.
    Stationary values have some jitter. We can smooth them but we need
    some confidence that distance is not changing.

    That's why we use partitioning. If we move sensor inside partition
    margins, sensor still be considered stationary. But when we do
    essential move out or partition boundaries, partition will be reset
    and jitter smoothing will start again.

  Concept entities:

    Span - float - Width of bucket. Distance between low and high
      margins.

    Value - float - Value to be added to bucket.

  Methods:

    Reset(Span) - Set new maximum span and reset margins.
    Add(Value) - Add value if it fits into span.

    GetCurrentSpan() - float - Get distance between low and high margins.
    GetMaxSpan() - float - Get maximum span.
    GetMinValue() - float - Get current low bucket margin.
    GetMaxValue() - float - Get current high bucket margin.

  Behavior:

    Bucket may change its margins after adding value.
    Guarantee is that current span will not surpass maximum span.
*/

#pragma once

#include <Arduino.h>

namespace me_Statistics_Bucket
{
  class Bucket
  {
    public:
      Bucket(float Span);
      void Reset(float Span);
      bool Add(float Value);
      float GetCurrentSpan();
      float GetMaxSpan();
      float GetMinValue();
      float GetMaxValue();

    private:
      float Span;
      float MinValue;
      float MaxValue;
  };
}
