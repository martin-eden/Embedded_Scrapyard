#pragma once

class PIDCSmoother {
  public:
    PIDCSmoother(float a_kP, float a_kI, float a_kD, float a_maxAccumulatedError);
    void addValue(float value);
    float getValue();
  private:
    float kP;
    float kI;
    float kD;
    float maxAccumulatedError;
    float accumulatedError;
    float lastError;
    float baseValue;
};

PIDCSmoother::PIDCSmoother(float a_kP, float a_kI, float a_kD, float a_maxAccumulatedError) {
  kP = a_kP;
  kI = a_kI;
  kD = a_kD;
  maxAccumulatedError = a_maxAccumulatedError;
  accumulatedError = 0;
  lastError = 0;
  baseValue = 0;
}

static inline float sign(float val) {
  if (val < 0)
    return -1.0;
  if (val == 0)
    return 0.0;
  return 1.0;
}

void PIDCSmoother::addValue(float value) {
  float error = value - baseValue;

  baseValue += kP * error;

  accumulatedError += error;
  baseValue += kI * accumulatedError;

  baseValue += kD * (error - lastError);
  lastError = value - baseValue;

  if (abs(accumulatedError) > maxAccumulatedError)
    accumulatedError = maxAccumulatedError * sign(accumulatedError);
}

float PIDCSmoother::getValue() {
  return baseValue;
}
