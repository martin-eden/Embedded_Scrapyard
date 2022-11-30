#pragma once

#include <Arduino.h>

/*
  Last mod.: 2022-11-30
*/

namespace me_Math_Scaling
{
  float FloatMap(float Value, float SrcRangeMin, float SrcRangeMax, float DestRangeMin, float DestRangeMax);
}
