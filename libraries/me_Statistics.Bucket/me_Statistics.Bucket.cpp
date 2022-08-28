#include "me_Statistics.Bucket.h"

using namespace me_Statistics_Bucket;

Bucket::Bucket(float Span = 1.0)
{
  Reset(Span);
}

void Bucket::Reset(float Span = 1.0)
{
  this->Span = Span;
  MinValue = 1e6;
  MaxValue = -MinValue;
}

bool Bucket::Add(float Value)
{
  float NewMinValue = min(MinValue, Value);
  float NewMaxValue = max(MaxValue, Value);

  bool Result = NewMaxValue - NewMinValue < Span;
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
