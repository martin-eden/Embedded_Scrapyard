#include "me_Math_Scaling.h"

/*
  Last mod.: 2022-11-30
*/

using namespace me_Math_Scaling;

float me_Math_Scaling::FloatMap(float Value, float SrcRangeMin, float SrcRangeMax, float DestRangeMin, float DestRangeMax)
{
  if (
    (SrcRangeMin >= SrcRangeMax) ||
    (DestRangeMin >= DestRangeMax)
  )
    return -1.0;

  Value = max(Value, SrcRangeMin);
  Value = min(Value, SrcRangeMax);

  float SrcOffset = Value - SrcRangeMin;
  float SrcSpan = SrcRangeMax - SrcRangeMin;
  float SrcPos = SrcOffset / SrcSpan;
  float DestPos = SrcPos;
  float DestSpan = DestRangeMax - DestRangeMin;
  float DestOffset = DestPos * DestSpan;
  float Result = DestOffset + DestRangeMin;

  return Result;
}
