#pragma once

#include <Arduino.h>

#include <me_DHT11.h>

namespace me_DHT11_StateGetter
{
  struct State {
    bool IsValid;
    float Temperature;
    float Humidity;
  };

  State GetState(me_DHT11::DHT11* sensor);
}
