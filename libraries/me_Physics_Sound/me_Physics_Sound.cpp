// Functions to calculate sound properties.

/*
  Status: first
  Last mod.: 2022-06-21
  Version: 1.0
*/

#include "me_Physics_Sound.h"

float me_Physics_Sound::GetDistanceFromEchoCm(uint32_t EchoDelayMcr)
{
  const float SoundSpeedMpS = 340.0;
  float distanceCm = EchoDelayMcr * 1e-6 * SoundSpeedMpS * 1e2 / 2;
  return distanceCm;
}
