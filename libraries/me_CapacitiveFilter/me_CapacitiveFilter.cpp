/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-08-28
*/

#include "me_CapacitiveFilter.h"

CapacitiveFilter::CapacitiveFilter(float a_capacitance, uint32_t a_autoDischargeSpan)
{
  accumulatedError = 0;
  stepsDone = 0;
  baseValue = 0;
  baseValueIsChanged = false;
  SetCapacitance(a_capacitance);
  SetAutoDischargeSpan(a_autoDischargeSpan);
}

void CapacitiveFilter::Add(float value)
{
  float error = value - baseValue;

  accumulatedError += error;

  ++stepsDone;

  if (stepsDone >= autoDischargeSpan)
    Flush();

  if (abs(accumulatedError) > capacitance)
    Flush();
  else
    baseValueIsChanged = false;
}

float CapacitiveFilter::Get()
{
  return baseValue;
}

void CapacitiveFilter::Flush()
{
  if (stepsDone == 0)
    return;

  float errorCorrection = accumulatedError / stepsDone;
  baseValueIsChanged = true;
  baseValue += errorCorrection;
  accumulatedError = 0;
  stepsDone = 0;
}

bool CapacitiveFilter::ValueHasChanged()
{
  return baseValueIsChanged;
}

void CapacitiveFilter::SetCapacitance(float a_capacitance)
{
  capacitance = a_capacitance;
}

float CapacitiveFilter::GetCapacitance()
{
  return capacitance;
}

float CapacitiveFilter::GetAccumulatedError()
{
  return accumulatedError;
}

void CapacitiveFilter::SetAutoDischargeSpan(uint32_t a_autoDischargeSpan)
{
  autoDischargeSpan = a_autoDischargeSpan;
}

uint32_t CapacitiveFilter::GetAutoDischargeSpan()
{
  return autoDischargeSpan;
}

