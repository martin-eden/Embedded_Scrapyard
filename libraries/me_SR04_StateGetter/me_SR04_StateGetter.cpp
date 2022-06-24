#include "me_SR04_StateGetter.h"

#include <me_SR04.h>
#include <me_Physics_Sound.h>
#include <me_CapacitiveFilter.h>

const float
  DataFilterCapacitance = 2.5;

CapacitiveFilter dataSmoother = CapacitiveFilter(DataFilterCapacitance);

using namespace me_SR04_StateGetter;

State me_SR04_StateGetter::GetState(me_SR04::SR04* sensor)
{
  State result;

  if (sensor->RequestStatus == me_SR04::ReadStatus::Success)
  {
    result.IsSensorWorking = true;
    result.HasDistance = true;

    float distance = me_Physics_Sound::GetDistanceFromEchoCm(sensor->EchoDelayMcr);
    dataSmoother.addValue(distance);
    distance = dataSmoother.getValue();

    result.DistanceCm = distance;
  }
  else
  {
    result.HasDistance = false;
    result.DistanceCm = 0.0;
    if (sensor->RequestStatus == me_SR04::ReadStatus::NoSignalStart)
    {
      result.IsSensorWorking = false;
    }
    else if (sensor->RequestStatus == me_SR04::ReadStatus::NoSignalEnd)
    {
      result.IsSensorWorking = true;
    }
  }

  return result;
}
