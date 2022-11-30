// Ultrasonic distance display for 128x64 monochrome OLED.

/*
  Version: 3
  Last mod.: 2022-12-03
*/

#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

#include <me_CommonDisplayWidget.h>

namespace me_SR04_StatePrinter_128x64
{
  class StatePrinter : public me_CommonDisplayWidget::Widget
  {
    public:
      StatePrinter(U8G2 *aScreen): Screen(aScreen)
      {
        Init(0, 0, 128, 64);
      };

      void Display(bool isConnected, bool hasDistance, float distanceCm);

    private:
      U8G2 *Screen;

      void DisplayDistance(float DistanceCm);
      void DisplayNotConnectedError();
      void DisplayNoDistanceError();
  };
}
