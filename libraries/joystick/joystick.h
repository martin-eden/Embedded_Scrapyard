// Joystick module from Keyes interface

/*
  Status: in writing
  Last mod.: 2016-04-28
*/

/*
  This joystick technically is two analog potentiometers and
  button (when stick pressed down). Connect x-y wires to analog
  pins, button to any digital pin. To read button correctly
  we need to use INPUT_PULLUP pin mode (or old-fashion
  "pinMode(pin, INPUT); digitalWrite(pin, HIGH);").

  When stick is self-centered x-y values I have is like
  (502, 508). I.e. not (512, 512) as in ideal world. I'm not
  addressing this issue now. In future library development some
  calibration must be supported.
*/

#pragma once

struct t_joystick_pins
{
  int axis_x;
  int axis_y;
  int button;
};

class c_joystick
{
  private:
    int stationary_x;
    int stationary_y;
  public:
    t_joystick_pins pins;
    bool invert_x;
    bool invert_y;
    void init();
    int get_raw_x();
    int get_raw_y();
    int get_scaled_x();
    int get_scaled_y();
    bool is_button_pressed();
    void calibrate();
};
