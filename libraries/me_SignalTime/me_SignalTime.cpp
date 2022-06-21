// Functions to measure signal duration.

#include <Arduino.h>

#include "me_SignalTime.h"

uint32_t me_SignalTime::GetLevelTime(uint8_t DataPin, uint8_t OriginalLevel, uint32_t LevelTimeoutMcr)
{
  uint8_t CurrentLevel;
  uint32_t TimePassedMcr;
  uint32_t StartTime = micros();
  bool IsTimeout;
  do
  {
    CurrentLevel = digitalRead(DataPin);
    TimePassedMcr = (micros() - StartTime);
    IsTimeout = (TimePassedMcr > LevelTimeoutMcr);
  }
  while ((CurrentLevel == OriginalLevel) && !IsTimeout);

  if (IsTimeout)
  {
    return 0;
  }

  return TimePassedMcr;
}

bool me_SignalTime::WaitWhileLevel(uint8_t DataPin, uint8_t OriginalLevel, uint32_t LevelTimeoutMcr)
{
  return (GetLevelTime(DataPin, OriginalLevel, LevelTimeoutMcr) != 0);
}
