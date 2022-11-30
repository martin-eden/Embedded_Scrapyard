// Distance getter for HC-SR04 sonar.

/*
  Version: 3
  Last mod.: 2022-12-01
*/

/*
  Fields

    bool NotConnected()
    bool HasDistance()

    float DistanceCm()
      Assumed signal medium is air at 20 deg C.

    void Update()

  Uses
    <me_SR04.h>
    <me_Physics_Sound.h>
*/

#include "me_SR04_Distance.h"

#include <me_SR04.h>
#include <me_Physics_Sound.h>

using namespace me_SR04_Distance;

void SensorDistance::Measure()
{
  Sonar->Ping();

  if (Sonar->NoSignalStart())
  {
    _NotConnected = true;
    _HasDistance = false;
    _DistanceCm = 0.0;
  }
  else if (Sonar->NoSignalEnd())
  {
    _NotConnected = false;
    _HasDistance = false;
    _DistanceCm = 0.0;
  }
  else
  {
    _NotConnected = false;
    _HasDistance = true;
    _DistanceCm = me_Physics_Sound::GetDistanceFromEchoCm(Sonar->EchoDelayMcr());
  }
}
