#include "me_Stat_AverageFilter.h"

using namespace me_Stat_AverageFilter;

void Filter::Reset()
{
  Average = 0.0;
  NumValues = 0;
}

void Filter::Add(float Value)
{
  if (NumValues < MaxNumValues)
    ++NumValues;
  Average -= Average / NumValues;
  Average += Value / NumValues;
}

float Filter::Get()
{
  return Average;
}
