// Functions to calculate sound properties.

/*
  Status: stable
  Last mod.: 2022-11-01
  Version: 1.0
*/

/*
  Usage

    float GetDistanceFromEchoCm(uint32_t EchoDelayMcr)

      Calculate distance from echo delay.

      * Assumed signal medium is air at 20 deg C.
      * Echo delay in microseconds.
      * Result in centimeters.
*/

#pragma once

#include <Arduino.h>

namespace me_Physics_Sound
{
  float GetDistanceFromEchoCm(uint32_t EchoDelayMcr);
}
