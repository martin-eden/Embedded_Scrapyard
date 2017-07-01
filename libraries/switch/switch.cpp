#include <Arduino.h>
#include "switch.h"

void c_switch::init() {
  pinMode(state_pin, OUTPUT);
  switch_off();
}

void c_switch::switch_on() {
  digitalWrite(state_pin, LOW);
  is_on = true;
}

void c_switch::switch_off() {
  digitalWrite(state_pin, HIGH);
  is_on = false;
}
