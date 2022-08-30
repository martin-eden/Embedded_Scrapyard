#include "me_Stat_AverageFilter.h"

using namespace me_Stat_AverageFilter;

void Filter::Reset()
{
  Average = 0.0;
  NumValues = 0;
}

void Filter::Add(float Value)
{
  Average = (Average * NumValues / (NumValues + 1)) + (Value / (NumValues + 1));
  if (NumValues < MaxNumValues)
    ++NumValues;
}

float Filter::Get()
{
  return Average;
}
