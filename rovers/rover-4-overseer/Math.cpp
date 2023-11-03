#include "Math.h"

float RoundToUnit(float Value, float UnitSize)
{
  return trunc(Value / UnitSize) * UnitSize;
}
