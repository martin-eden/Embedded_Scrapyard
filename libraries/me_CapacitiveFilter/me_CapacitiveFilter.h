#pragma once

#include <Arduino.h>

class CapacitiveFilter
{
  public:
    CapacitiveFilter(float a_capacitance, uint32_t a_autoDischargeSpan = 100);
    void Add(float value);
    void Flush();
    float Get();
    bool ValueHasChanged();

    void SetCapacitance(float a_capacitance);
    float GetCapacitance();

    float GetAccumulatedError();

    void SetAutoDischargeSpan(uint32_t a_autoDischargeSpan);
    uint32_t GetAutoDischargeSpan();

  private:
    float capacitance;
    float accumulatedError;
    uint32_t stepsDone;
    float baseValue;
    bool baseValueIsChanged;
    uint32_t autoDischargeSpan;
};
