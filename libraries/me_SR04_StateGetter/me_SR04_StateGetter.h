// Higher-level wrapper for <me_SR04> ultrasonic sensor.

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-11-01
*/

/*
  Usage

    State GetState(me_SR04::SR04* sensor)

      Convert state of <me_SR04> sensor to more human-manageable
      format.

      To get distance you should first call <me_SR04>.Ping() to
      set sensor state.

      Returns <State> record.

    struct State

      bool IsSensorWorking

        TRUE when sensor is connected.

        Technically it means that we saw EchoPin go HIGH.

      bool HasDistance

        TRUE when we have some meaningful distance.

        Technically it means that we saw end of EchoPin HIGH state
        before hardware timeout.

        Only has meaning when <IsSensorWorking>.

      float DistanceCm

        Distance in centimeters.

        Assumed signal medium is air at 20 deg C.

        Only has meaning when <HasDistance>.
*/

#pragma once

#include <Arduino.h>

#include <me_SR04.h>

namespace me_SR04_StateGetter
{
  struct State {
    bool IsSensorWorking;
    bool HasDistance;
    float DistanceCm;
  };

  State GetState(me_SR04::SR04* sensor);
}
