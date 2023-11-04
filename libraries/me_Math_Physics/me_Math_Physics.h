// Functions to calculate sound properties.

/*
  Status: stable
  Last mod.: 2023-11-04
  Version: 1
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

float GetDistanceFromEcho_Cm(uint32_t EchoDelay_Us);

/*
  2022-11-01
  2023-11-04
*/
