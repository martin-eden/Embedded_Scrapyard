// Higher-level wrapper for <me_SR04> ultrasonic sensor.

/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-11-13
*/

#include "me_SR04_StateGetter.h"

#include <me_SR04.h>
#include <me_Physics_Sound.h>

using namespace me_SR04_StateGetter;

State me_SR04_StateGetter::GetState(me_SR04::SR04* sensor)
{
  State result;

  result.IsConnected = false;
  result.HasDistance = false;
  result.DistanceCm = 0.0;

  using namespace me_SR04;

  if (sensor->RequestStatus == Status::Success)
  {
    result.IsConnected = true;
    result.HasDistance = true;
    result.DistanceCm = me_Physics_Sound::GetDistanceFromEchoCm(sensor->EchoDelayMcr);
  }
  else if (sensor->RequestStatus == Status::NoSignalEnd)
  {
    result.IsConnected = true;
  }
  else if (sensor->RequestStatus == Status::NoSignalStart)
  {
  }

  return result;
}
