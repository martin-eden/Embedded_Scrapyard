// Display sonar distance results on 128x32 OLED.

/*
  Status: stable
  Version: 1.2
  Last mod.: 2022-08-30
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
    WidgetX = 116,
    WidgetY = 4,
    WidgetHeight = 25;

  Display->drawVLine(WidgetX, WidgetY, WidgetHeight);
}

void StatePrinter::DisplayDistance(float DistanceCm)
{
  const u8g2_uint_t
    WidgetX = 18,
    WidgetY = 23;

  char Buffer[16];
  String(String(DistanceCm, 1) + " cm").toCharArray(Buffer, sizeof(Buffer));

  Display->setFont(u8g2_font_profont22_tf);
  Display->drawStr(WidgetX, WidgetY, Buffer);
}

float FloatMap(float Value, float SrcRangeMin, float SrcRangeMax, float DestRangeMin, float DestRangeMax)
{
  if (
    (SrcRangeMin >= SrcRangeMax) ||
    (DestRangeMin >= DestRangeMax)
  )
    return -1.0;

  Value = max(Value, SrcRangeMin);
  Value = min(Value, SrcRangeMax);

  float SrcOffset = Value - SrcRangeMin;
  float SrcSpan = SrcRangeMax - SrcRangeMin;
  float SrcPos = SrcOffset / SrcSpan;
  float DestPos = SrcPos;
  float DestSpan = DestRangeMax - DestRangeMin;
  float DestOffset = DestPos * DestSpan;
  float Result = DestOffset + DestRangeMin;

  return Result;
}

void StatePrinter::DisplayFineMark(float DistanceCm)
{
  const u8g2_uint_t
    WidgetX = 2,
    WidgetY = 2,
    WidgetWidth = 124,
    WidgetHeight = 28,
    WidgetToothHeight = 4;

  const float
    BaseUnitCm = 5.0;

  float BaseCm = trunc(DistanceCm / BaseUnitCm) * BaseUnitCm;

  u8g2_uint_t X =
    FloatMap(
      DistanceCm,
      BaseCm,
      BaseCm + BaseUnitCm,
      WidgetX,
      WidgetX + WidgetWidth
    );

  // Display->drawVLine(X, WidgetY, WidgetHeight);

  Display->drawVLine(X, WidgetY, WidgetToothHeight);
  Display->drawVLine(X, WidgetY + WidgetHeight - WidgetToothHeight, WidgetToothHeight);
}

void StatePrinter::DisplayFlipFlop()
{
  const u8g2_uint_t
    WidgetX = 118,
    WidgetY = 13,
    WidgetWidth = 8,
    WidgetHeight = 8;

  static bool flip = false;
  if (flip)
  {
    Display->drawBox(WidgetX, WidgetY, WidgetWidth, WidgetHeight);
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
  DisplayOuterFrame();

  if (DataState.HasDistance)
  {
    // DisplayGridLines();
    DisplayFineMark(DataState.DistanceCm);
    DisplayDistance(DataState.DistanceCm);
    // DisplayFlipFlop();
  }
  else if (!DataState.IsSensorWorking)
    DisplayNotConnectedError();
  else if (!DataState.HasDistance)
    DisplayNoDistanceError();
  else
    exit(1);
}
