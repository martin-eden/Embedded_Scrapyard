/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-07-07
*/

#include "me_CapacitiveFilter.h"

CapacitiveFilter::CapacitiveFilter(float a_maxAccumulatedError) {
  maxAccumulatedError = a_maxAccumulatedError;
  accumulatedError = 0;
  stepsDone = 0;
  baseValue = 0;
  baseValueIsChanged = false;
}

void CapacitiveFilter::Add(float value) {
  float error = value - baseValue;

  accumulatedError += error;

  ++stepsDone;
  if (abs(accumulatedError) > maxAccumulatedError) {
    float errorCorrection = accumulatedError / stepsDone;
    baseValueIsChanged = true;
    baseValue += errorCorrection;
    accumulatedError = 0;
    stepsDone = 0;
  }
  else
    baseValueIsChanged = false;
}

float CapacitiveFilter::Get() {
  return baseValue;
}

bool CapacitiveFilter::ValueHasChanged() {
  return baseValueIsChanged;
}
