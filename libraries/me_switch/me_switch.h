#pragma once

class c_switch {
  public:
    bool is_on;
    int state_pin;
    void init();
    void switch_on();
    void switch_off();
};
