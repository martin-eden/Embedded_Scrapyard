#pragma once

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-07-07
*/

class CapacitiveFilter {
  public:
    CapacitiveFilter(float a_maxAccumulatedError);
    void Add(float value);
    float Get();
    bool ValueHasChanged();
  private:
    float maxAccumulatedError;
    float accumulatedError;
    uint32_t stepsDone;
    float baseValue;
    bool baseValueIsChanged;
};

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
