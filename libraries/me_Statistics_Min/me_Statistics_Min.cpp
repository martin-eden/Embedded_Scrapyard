#include "me_Statistics_Min.h"

using namespace me_Statistics_TimePoint;
using namespace me_Statistics_Min;

void MinValue::Add(TimePoint aTimePoint)
{
  if (aTimePoint.Value < timePoint.Value)
  {
    timePoint = aTimePoint;
    isUpdated = true;
  }
  else
    isUpdated = false;
}

TimePoint MinValue::Get()
{
  return timePoint;
}

bool MinValue::ValueHasChanged()
{
  return isUpdated;
}
