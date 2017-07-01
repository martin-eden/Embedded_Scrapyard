// Simple joystick values dumper

/*
  Status: done
  Last mod.: 2016-04-28
*/

// Used for class usage testing and device test.

#include "joystick.h"

const t_joystick_pins joystick_pins = {A1, A0, 4};
c_joystick joystick;

void setup()
{
  joystick.pins = joystick_pins;
  joystick.init();
  Serial.begin(9600);
}

void loop()
{
  int raw_x = joystick.get_raw_x();
  int raw_y = joystick.get_raw_y();
  int x = joystick.get_scaled_x();
  int y = joystick.get_scaled_y();
  bool button_pressed = joystick.is_button_pressed();

  Serial.print("x: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.print("y: ");
  Serial.print(y);
  Serial.print(" (");
  Serial.print(raw_x);
  Serial.print(", ");
  Serial.print(raw_y);
  Serial.print(")");
  Serial.print(", ");
  Serial.print("button: ");
  if (button_pressed)
    Serial.print("ON");
  else
    Serial.print("OFF");
  Serial.print("");
  Serial.print("\n");
  delay(1500);
}
