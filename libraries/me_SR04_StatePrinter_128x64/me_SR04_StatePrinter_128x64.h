// Ultrasonic distance display for 128x64 monochrome OLED.

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-11-06
*/

#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

#include <me_CommonDisplayWidget.h>

#include <me_SR04_StateGetter.h>

namespace me_SR04_StatePrinter_128x64
{
  class StatePrinter : public me_CommonDisplayWidget::Widget
  {
    public:
      StatePrinter(U8G2* aScreen);

      void Display(me_SR04_StateGetter::State State);

    private:
      U8G2* Screen;

      void DisplayDistance(float DistanceCm);
      void DisplayNotConnectedError();
      void DisplayNoDistanceError();
  };
}
