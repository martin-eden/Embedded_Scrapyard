// Ultrasonic distance display for 128x64 monochrome OLED.

/*
  Status: stable
  Version: 0.9
  Last mod.: 2022-11-07
*/

#include "me_SR04_StatePrinter_128x64.h"

#include <me_SR04_StateGetter.h>

using namespace me_SR04_StatePrinter_128x64;

StatePrinter::StatePrinter(U8G2* aScreen)
{
  Init(0, 0, 128, 64);

  Screen = aScreen;
}

void StatePrinter::DisplayDistance(float DistanceCm)
{
  const uint8_t BufferSize = 16;
  char Buffer[BufferSize];

  uint16_t DistanceInt = DistanceCm;
  uint8_t DistanceFrac = uint32_t(DistanceCm * 10) % 10;
  snprintf(Buffer, BufferSize, "%d.%d", DistanceInt, DistanceFrac);

  uint16_t NumericLineWidth = Screen->getStrWidth(Buffer);

  strcat(Buffer, " cm");

  uint16_t TextWidth = Screen->getStrWidth(Buffer);
  uint16_t TextHeight = Screen->getAscent() + Screen->getDescent();

  uint16_t TextX = GetCenterX() - (TextWidth / 2);
  uint16_t TextY = GetCenterY() + (TextHeight / 2);

  const uint16_t LineOffsetY = 4;
  const uint16_t LineOverdrawX = 2;

  NumericLineWidth += LineOverdrawX * 2;
  uint16_t NumericLineX = TextX - LineOverdrawX;
  uint16_t NumericLineY = TextY + LineOffsetY;

  Screen->setFont(u8g2_font_profont22_tf);
  Screen->drawStr(TextX, TextY, Buffer);
  Screen->drawHLine(MapX(NumericLineX), MapY(NumericLineY), NumericLineWidth);
}

void StatePrinter::DisplayNotConnectedError()
{
  const u8g2_uint_t
    X = 6,
    Y = 38;

  Screen->setFont(u8g2_font_profont22_tf);
  Screen->drawStr(MapX(X), MapY(Y), "not conn");
}

void StatePrinter::DisplayNoDistanceError()
{
  const u8g2_uint_t
    X = 6,
    Y = 38;

  Screen->setFont(u8g2_font_profont22_tf);
  Screen->drawStr(MapX(X), MapY(Y), "no dist");
}

void StatePrinter::Display(me_SR04_StateGetter::State DataState)
{
  if (DataState.HasDistance)
    DisplayDistance(DataState.DistanceCm);
  else if (!DataState.IsSensorWorking)
    DisplayNotConnectedError();
  else if (!DataState.HasDistance)
    DisplayNoDistanceError();
  else
    exit(1);
}
