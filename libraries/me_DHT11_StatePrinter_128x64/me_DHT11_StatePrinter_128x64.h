#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

#include <me_DHT11_StateGetter.h>

namespace me_DHT11_StatePrinter_128x64
{
  class StatePrinter
  {
    public:
      StatePrinter(U8G2* aDisplay) : Display(aDisplay) {};

      void Print(me_DHT11_StateGetter::State state);

    private:
      U8G2* Display;

      void DisplayOuterFrame();
      void DisplayGridLines();
      void DisplayTemperature(float Temperature);
      void DisplayHumidity(float Humidity);
      void DisplayFlipFlop();
      void DisplayReadError();
  };
}
