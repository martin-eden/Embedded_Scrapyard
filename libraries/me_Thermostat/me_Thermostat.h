#pragma once

#include <me_switch.h>

class cThermostat {
  public:
    float min_value, max_value;
    c_switch relay;
    bool enabling_increases_value;
    cThermostat(c_switch aRelay):
      min_value(22.5),
      max_value(23.5),
      enabling_increases_value(true),
      relay(aRelay) {
      }
    void process(float temperature) {
      /*
      String msg = "";
      msg =
        msg +
        "temperature: " + String(temperature, 2) +
        " " +
        "relay.is_off(): " + relay.is_off() +
        " " +
        "min_value: " + String(min_value, 2) +
        " " +
        "max_value: " + String(max_value, 2) +
        " " +
        "enabling_increases_value: " + String(enabling_increases_value)
        ;

      Serial.println(msg);
      //*/

      if (
        relay.is_off() and
        (
          (enabling_increases_value and (temperature < min_value)) or
          (!enabling_increases_value and (temperature > max_value))
        )
      ) {
          relay.switch_on();
          // Serial.println("Switch ON");
      } else if (
        relay.is_on() and
        (
          (enabling_increases_value and (temperature > max_value)) or
          (!enabling_increases_value and (temperature < min_value))
        )
      ) {
          relay.switch_off();
          // Serial.println("Switch OFF");
      }
    }
};
