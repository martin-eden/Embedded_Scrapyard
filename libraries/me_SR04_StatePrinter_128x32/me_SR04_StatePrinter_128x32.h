#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

#include <me_SR04_StateGetter.h>

namespace me_SR04_StatePrinter_128x32
{
  class StatePrinter
  {
    public:
      StatePrinter(U8G2* aDisplay) : Display(aDisplay) {};

      void Print(me_SR04_StateGetter::State state);

    private:
      U8G2* Display;

      void DisplayOuterFrame();
      void DisplayGridLines();
      void DisplayDistance(float DistanceCm);
      void DisplayFineMark(float DistanceCm);
      void DisplayFlipFlop();
      void DisplayNotConnectedError();
      void DisplayNoDistanceError();
  };
}
