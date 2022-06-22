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
