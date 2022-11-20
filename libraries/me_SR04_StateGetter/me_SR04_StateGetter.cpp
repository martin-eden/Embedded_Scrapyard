// Higher-level wrapper for <me_SR04> ultrasonic sensor.

/*
  Status: stable
  Version: 1.2
  Last mod.: 2022-11-20
*/

#include "me_SR04_StateGetter.h"

#include <me_SR04.h>
#include <me_Physics_Sound.h>

using namespace me_SR04_StateGetter;

State me_SR04_StateGetter::GetState(me_SR04::SR04* Sensor)
{
  State Result;

  if (Sensor->RequestStatus == me_SR04::Status::Success)
  {
    Result.IsConnected = true;
    Result.HasDistance = true;
    Result.DistanceCm = me_Physics_Sound::GetDistanceFromEchoCm(Sensor->EchoDelayMcr);
  }
  else if (Sensor->RequestStatus == me_SR04::Status::NoSignalStart)
  {
    Result.IsConnected = false;
    Result.HasDistance = false;
    Result.DistanceCm = 0.0;
  }
  else if (Sensor->RequestStatus == me_SR04::Status::NoSignalEnd)
  {
    Result.IsConnected = true;
    Result.HasDistance = false;
    Result.DistanceCm = 0.0;
  }

  return Result;
}
