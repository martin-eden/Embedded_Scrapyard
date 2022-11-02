// Functions to calculate sound properties.

/*
  Status: stable
  Last mod.: 2022-11-01
  Version: 1.1
*/

#include "me_Physics_Sound.h"

float me_Physics_Sound::GetDistanceFromEchoCm(uint32_t EchoDelayMcr)
{
  // https://en.wikipedia.org/wiki/Speed_of_sound
  const float SoundSpeedMpS = 343.0;

  float distanceCm = ((EchoDelayMcr / 2) * 1e-6) * SoundSpeedMpS * 1e2;

  return distanceCm;
}
