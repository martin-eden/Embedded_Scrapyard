// Display sonar distance results on 128x32 OLED.

/*
  Status: in writing
  Version: 1.0
  Last mod.: 2022-06-21
*/

#include "me_SR04_StatePrinter_128x32.h"

#include <U8g2lib.h>
#include <me_SR04_StateGetter.h>

using namespace me_SR04_StatePrinter_128x32;

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
    WidgetX = 123,
    WidgetY = 4,
    WidgetHeight = 25;

  Display->drawVLine(WidgetX, WidgetY, WidgetHeight);
}

void StatePrinter::DisplayDistance(float DistanceCm)
{
  const u8g2_uint_t
    WidgetX = 25,
    WidgetY = 23;

  char Buffer[16];
  String(String(DistanceCm, 1) + " cm").toCharArray(Buffer, sizeof(Buffer));

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

void StatePrinter::DisplayNotConnectedError()
{
  const u8g2_uint_t
    WidgetX = 6,
    WidgetY = 22;

  Display->setFont(u8g2_font_profont22_tf);
  Display->drawStr(WidgetX, WidgetY, "not conn");
}

void StatePrinter::DisplayNoDistanceError()
{
  const u8g2_uint_t
    WidgetX = 6,
    WidgetY = 22;

  Display->setFont(u8g2_font_profont22_tf);
  Display->drawStr(WidgetX, WidgetY, "no dist");
}

void StatePrinter::Print(me_SR04_StateGetter::State DataState)
{
  if (DataState.HasDistance)
  {
    DisplayOuterFrame();
    DisplayGridLines();
    DisplayDistance(DataState.DistanceCm);
    DisplayFlipFlop();
  }
  else if (!DataState.IsSensorWorking)
  {
    DisplayOuterFrame();
    DisplayNotConnectedError();
  }
  else if (!DataState.HasDistance)
  {
    DisplayOuterFrame();
    DisplayNoDistanceError();
  }
  else
    exit(1);
}
