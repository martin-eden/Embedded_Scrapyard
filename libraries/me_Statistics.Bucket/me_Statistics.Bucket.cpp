// Magnetic bucket implementation

/*
  Status: stable
  Version: 0.8
  Last mod.: 2022-10-09
*/

#include "me_Statistics.Bucket.h"

using namespace me_Statistics_Bucket;

Bucket::Bucket(float Span)
{
  Reset(Span);
}

void Bucket::Reset(float Span)
{
  this->Span = Span;
  MinValue = 1e6;
  MaxValue = -MinValue;
}

bool Bucket::Add(float Value)
{
  float NewMinValue = min(MinValue, Value);
  float NewMaxValue = max(MaxValue, Value);

  bool Result = ((NewMaxValue - NewMinValue) < Span);
  if (Result)
  {
    MinValue = NewMinValue;
    MaxValue = NewMaxValue;
  }

  return Result;
}

float Bucket::GetCurrentSpan()
{
  return (MaxValue - MinValue);
}

float Bucket::GetMaxSpan()
{
  return Span;
}

float Bucket::GetMinValue()
{
  return MinValue;
}

float Bucket::GetMaxValue()
{
  return MaxValue;
}
