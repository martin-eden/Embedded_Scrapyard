#pragma once

#include <Arduino.h>

class me_DataLogger
{
  public:
    me_DataLogger(uint8_t a_cs_pin);
    void writeString(String s);
  private:
    uint8_t cs_pin;
};
