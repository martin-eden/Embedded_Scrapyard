#pragma once

class CapacitiveFilter {
  public:
    CapacitiveFilter(float a_maxAccumulatedError);
    void addValue(float value);
    float getValue();
  private:
    float maxAccumulatedError;
    float accumulatedError;
    uint32_t stepsDone;
    float baseValue;
};

CapacitiveFilter::CapacitiveFilter(float a_maxAccumulatedError) {
  maxAccumulatedError = a_maxAccumulatedError;
  accumulatedError = 0;
  stepsDone = 0;
  baseValue = 0;
}

void CapacitiveFilter::addValue(float value) {
  float error = value - baseValue;

  accumulatedError += error;

  ++stepsDone;
  if (abs(accumulatedError) > maxAccumulatedError) {
    baseValue += (accumulatedError / stepsDone);
    accumulatedError = 0;
    stepsDone = 0;
  }
}

float CapacitiveFilter::getValue() {
  return baseValue;
}
