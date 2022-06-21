// Functions to measure signal duration.

namespace me_SignalTime
{
  uint32_t GetLevelTime(uint8_t DataPin, uint8_t OriginalLevel, uint32_t LevelTimeoutMcr);
  bool WaitWhileLevel(uint8_t DataPin, uint8_t OriginalLevel, uint32_t LevelTimeoutMcr);
}
