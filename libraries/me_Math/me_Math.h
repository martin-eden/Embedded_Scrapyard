#pragma once

#include <Arduino.h>

float RoundToUnit(float Value, float UnitSize);
float RoundToDigits(float Value, int8_t LastSignificantDigitPosition = -2);
float RadToDeg(float Radians);
float DegToRad(float Degrees);

/*
  2023-11-03
  2023-11-04
  2024-03-23
*/
