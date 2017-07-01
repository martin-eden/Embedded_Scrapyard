// Joystick module from Keyes implementation

/*
  Status: stable
  Last mod.: 2016-04-28
*/

#include <Arduino.h>
#include "joystick.h"

int c_joystick::get_raw_x()
{
  int result;
  result = analogRead(pins.axis_x);
  return result;
}

int c_joystick::get_raw_y()
{
  int result;
  result = analogRead(pins.axis_y);
  return result;
}

void c_joystick::calibrate()
{
  stationary_x = get_raw_x();
  stationary_y = get_raw_y();
}

void c_joystick::init()
{
  pinMode(pins.button, INPUT_PULLUP);
  calibrate();
}

bool c_joystick::is_button_pressed()
{
  bool result;
  result = (digitalRead(pins.button) == LOW);
  return result;
}

int c_joystick::get_scaled_x()
{
  int result;
  int raw_x = get_raw_x();
  if (raw_x < stationary_x)
    result = map(raw_x, 0, stationary_x, -100, 0);
  else
    result = map(raw_x, stationary_x, 1023, 0, 100);
  if (invert_x)
    result = -result;
  return result;
}

int c_joystick::get_scaled_y()
{
  int result;
  int raw_y = get_raw_y();
  if (raw_y < stationary_y)
    result = map(raw_y, 0, stationary_y, -100, 0);
  else
    result = map(raw_y, stationary_y, 1023, 0, 100);
  if (invert_y)
    result = -result;
  return result;
}
