#pragma once

class c_switch {
  public:
    c_switch(uint8_t state_pin);
    void switch_on();
    void switch_off();
    bool is_on();
    bool is_off();
  private:
    bool _is_on;
    int _state_pin;
    void init();
};
