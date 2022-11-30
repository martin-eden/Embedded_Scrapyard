// Distance getter for HC-SR04 ultrasonic sensor.

/*
  Version: 4
  Last mod.: 2022-12-03
*/

/*
  This implementation uses SR04 driver from <me_SR04.h>.

  Fields

    void Measure()

      Ping on SR04 device, convert echo delay from microseconds to
      centimeters, fill class private fields.

    bool HasDistance();
      TRUE when we have some meaningful distance.

    float DistanceCm();
      Distance in centimeters.

      Assumed signal medium is air at 20 deg C.

      Value is undefined if <!HasDistance>.

  Uses

    <me_SR04.h>
*/

#pragma once

#include <Arduino.h>

#include <me_SR04.h>

namespace me_SR04_Distance
{
  class SensorDistance
  {
    public:
      SensorDistance(me_SR04::Sonar *aSonar):
        Sonar(aSonar) {};

      void Measure();

      bool NotConnected() { return _NotConnected; };
      bool HasDistance() { return _HasDistance; };
      float DistanceCm() { return _DistanceCm; };

    private:
      me_SR04::Sonar *Sonar;

      bool _NotConnected;
      bool _HasDistance;
      float _DistanceCm;
  };
}
