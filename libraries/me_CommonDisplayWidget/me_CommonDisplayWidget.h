// Common widget display class. Screen rectangle.

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-11-06
*/

#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

namespace me_CommonDisplayWidget
{
  class Widget
  {
    public:
      void Init(
        uint16_t aBaseX,
        uint16_t aBaseY,
        uint16_t aWidth,
        uint16_t aHeight
      )
      {
        BaseX = aBaseX;
        BaseY = aBaseY;
        Width = aWidth;
        Height = aHeight;
      };

      uint16_t MapX(uint16_t X) { return (BaseX + X); };
      uint16_t MapY(uint16_t Y) { return (BaseY + Y); };

      uint16_t GetCenterX() { return (Width / 2); };
      uint16_t GetCenterY() { return (Height / 2); };

      uint16_t GetX() { return BaseX; };
      uint16_t GetY() { return BaseY; };
      uint16_t GetWidth() { return Width; };
      uint16_t GetHeight() { return Height; };


    protected:
      uint16_t BaseX;
      uint16_t BaseY;
      uint16_t Width;
      uint16_t Height;
  };
}
