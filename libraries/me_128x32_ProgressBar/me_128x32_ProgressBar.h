#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

#include <me_DHT11.h>

class me_128x32_ProgressBar
{
  public:
    U8G2* Display;

    int32_t MinValue = 0;
    int32_t MaxValue = 1023;
    int32_t CurrentValue = 0;
    uint8_t Left, Top, Right, Bottom;

    me_128x32_ProgressBar(
      U8G2* aDisplay,
      uint8_t aLeft = 0,
      uint8_t aTop = 0,
      uint8_t aRight = 127,
      uint8_t aBottom = 31
    );

    void Draw();

  private:
    void DrawOuterBox();
    void DrawFill();
};
