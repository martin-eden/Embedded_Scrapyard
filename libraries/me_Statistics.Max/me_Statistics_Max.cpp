#include "me_Statistics_Max.h"

using namespace me_Statistics_TimePoint;
using namespace me_Statistics_Max;

void MaxValue::Add(TimePoint aTimePoint)
{
  if (aTimePoint.Value > timePoint.Value)
  {
    timePoint = aTimePoint;
    isUpdated = true;
  }
  else
    isUpdated = false;
}

TimePoint MaxValue::Get()
{
  return timePoint;
}

bool MaxValue::ValueHasChanged()
{
  return isUpdated;
}
