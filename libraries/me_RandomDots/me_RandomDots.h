#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

class me_RandomDots
{
  public:
    U8G2* Display;

    uint16_t MaxFillRatio = 1023;
    uint16_t CurrentFillRatio = 0;
    uint8_t Left, Top, Right, Bottom;

    me_RandomDots(
      U8G2* aDisplay,
      uint8_t aLeft = 0,
      uint8_t aTop = 0,
      uint8_t aRight = 127,
      uint8_t aBottom = 31
    );

    void Draw();
};
