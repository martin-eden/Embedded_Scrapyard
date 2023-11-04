#pragma once

#include <Arduino.h>

float RoundToUnit(float Value, float UnitSize);
float RoundToDigits(float Value, int8_t LastSignificantDigitPosition = -2);
float RadToDeg(float Value);
