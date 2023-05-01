// Custom display DHT11 hygrometer data for 128x32 OLED display.

/*
  Status: stable
  Version: 1.0
  Last mod.: 2023-03-30
*/

/*
  Forked from [me_DHT11_StatePrinter_128x32] 2023-03-30.
*/

#include "me_DHT11_StatePrinter_128x64.h"

#include <U8g2lib.h>
#include <me_DHT11_StateGetter.h>

using namespace me_DHT11_StatePrinter_128x64;

void StatePrinter::DisplayOuterFrame()
{
  const u8g2_uint_t
    WidgetX = 0,
    WidgetY = 0,
    WidgetWidth = 128,
    WidgetHeight = 32;

  Display->drawFrame(WidgetX, WidgetY, WidgetWidth, WidgetHeight);
}

void StatePrinter::DisplayGridLines()
{
  const u8g2_uint_t
    WidgetX1 = 80,
    WidgetX2 = 123,
    WidgetY = 4,
    WidgetHeight = 25;

  Display->drawVLine(WidgetX1, WidgetY, WidgetHeight);
  Display->drawVLine(WidgetX2, WidgetY, WidgetHeight);
}

void StatePrinter::DisplayTemperature(float Temperature)
{
  const u8g2_uint_t
    WidgetX = 5,
    WidgetY = 23;

  char Buffer[8];
  String(String(Temperature, 1) + "\260C").toCharArray(Buffer, sizeof(Buffer));

  Display->setFont(u8g2_font_profont22_tf);
  Display->drawStr(WidgetX, WidgetY, Buffer);
}

void StatePrinter::DisplayHumidity(float Humidity)
{
  const u8g2_uint_t
    WidgetX = 85,
    WidgetY = 23;

  char Buffer[8];
  String(String(Humidity, 0) + "%").toCharArray(Buffer, sizeof(Buffer));

  Display->setFont(u8g2_font_profont22_tf);
  Display->drawStr(WidgetX, WidgetY, Buffer);
}

void StatePrinter::DisplayFlipFlop()
{
  const u8g2_uint_t
    WidgetX = 125,
    WidgetY = 13,
    WidgetHeight = 8;

  static bool flip = false;
  if (flip)
  {
    Display->drawVLine(WidgetX, WidgetY, WidgetHeight);
  }

  flip = !flip;
}

void StatePrinter::DisplayReadError()
{
  const u8g2_uint_t
    WidgetX = 6,
    WidgetY = 22;

  Display->setFont(u8g2_font_profont22_tf);
  Display->drawStr(WidgetX, WidgetY, "read error");
}

void StatePrinter::Print(me_DHT11_StateGetter::State DataState)
{
  if (DataState.IsValid)
  {
    DisplayOuterFrame();
    DisplayGridLines();
    DisplayTemperature(DataState.Temperature);
    DisplayHumidity(DataState.Humidity);
    DisplayFlipFlop();
  }
  else
  {
    DisplayOuterFrame();
    DisplayReadError();
  }
}
