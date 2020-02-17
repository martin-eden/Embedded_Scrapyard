#pragma once

#include <me_switch.h>

class cThermostat {
  public:
    float setpoint;
    float spread;
    c_switch relay;
    cThermostat(c_switch aRelay):
      setpoint(23),
      spread(1),
      relay(aRelay)
      {}
    virtual void process(float temperature);
};

class cHeatHoldingThermostat : public cThermostat {
  public:
    using cThermostat::cThermostat;
    void process(float temperature) {
      /*
      String msg = "";
      msg =
        msg +
        "temperature: " + String(temperature, 2) +
        " " +
        "relay.is_off(): " + relay.is_off() +
        " " +
        "setpoint: " + String(setpoint, 2) +
        " " +
        "spread: " + String(spread, 2);

      Serial.println(msg);
      */
      if (relay.is_off() and (temperature < setpoint - spread / 2)) {
        relay.switch_on();
        // Serial.println("Thermostat ON");
      }
      if (relay.is_on() and (temperature > setpoint + spread / 2)) {
        relay.switch_off();
        // Serial.println("Thermostat OFF");
      }
    };
};
