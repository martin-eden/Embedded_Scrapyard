#pragma once

#include <Arduino.h>

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
