// Magnetic bucket implementation

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-10-10
*/

#include "me_Statistics.Bucket.h"
#include <float.h>

using namespace me_Statistics_Bucket;

Bucket::Bucket(float Span)
{
  SetMaxSpan(Span);
}

void Bucket::SetMaxSpan(float Span)
{
  this->MaxSpan = Span;
  this->MinValue = FLT_MAX;
  this->MaxValue = -FLT_MAX;
}

bool Bucket::Add(float Value)
{
  float NewMinValue = min(MinValue, Value);
  float NewMaxValue = max(MaxValue, Value);

  float NewSpan = CalculateSpan(NewMinValue, NewMaxValue);

  bool Result = (NewSpan < GetMaxSpan());
  if (Result)
  {
    MinValue = NewMinValue;
    MaxValue = NewMaxValue;
  }

  return Result;
}

float Bucket::GetCurSpan()
{
  return CalculateSpan(MinValue, MaxValue);
}

float Bucket::GetMaxSpan()
{
  return this->MaxSpan;
}

float Bucket::GetMinValue()
{
  return this->MinValue;
}

float Bucket::GetMaxValue()
{
  return this->MaxValue;
}

float Bucket::CalculateSpan(float MinValue, float MaxValue)
{
  return (MaxValue - MinValue);
}
