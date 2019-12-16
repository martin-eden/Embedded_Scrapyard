#include <Arduino.h>
#include "me_switch.h"

c_switch::c_switch(uint8_t state_pin)
{
  _state_pin = state_pin;
  init();
}

void c_switch::init()
{
  pinMode(_state_pin, OUTPUT);
  switch_off();
}

void c_switch::switch_on()
{
  digitalWrite(_state_pin, LOW);
  _is_on = true;
}

void c_switch::switch_off()
{
  digitalWrite(_state_pin, HIGH);
  _is_on = false;
}

bool c_switch::is_on()
{
  return _is_on;
}

bool c_switch::is_off()
{
  return !_is_on;
}
