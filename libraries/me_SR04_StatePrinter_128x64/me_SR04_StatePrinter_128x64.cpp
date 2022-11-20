// Ultrasonic distance display for 128x64 monochrome OLED.

/*
  Status: stable
  Version: 1.2
  Last mod.: 2022-11-20
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

  uint16_t TextX;
  uint16_t TextY;
  uint16_t TextWidth;
  uint16_t TextHeight;

  uint16_t UnderscoreWidth;
  const uint16_t UnderscoreHeight = 2;
  uint16_t UnderscoreX;
  uint16_t UnderscoreY;
  const uint16_t UnderscoreOffsetY = 4;
  const uint16_t UnderscoreOverdrawX = 2;

  uint16_t DistanceInt;
  uint8_t DistanceFrac;

  char Buffer[BufferSize];

  DistanceInt = DistanceCm;
  DistanceFrac = uint32_t(DistanceCm * 10) % 10;

  snprintf(Buffer, BufferSize, "%d.%d", DistanceInt, DistanceFrac);

  Screen->setFont(u8g2_font_profont22_tf);

  UnderscoreWidth = Screen->getStrWidth(Buffer);

  strcat(Buffer, " cm");

  TextWidth = Screen->getStrWidth(Buffer);
  TextHeight = Screen->getAscent() + Screen->getDescent();

  TextX = GetCenterX() - (TextWidth / 2);
  TextY = GetCenterY() + (TextHeight / 2);

  UnderscoreWidth += UnderscoreOverdrawX * 2;
  UnderscoreX = TextX - UnderscoreOverdrawX;
  UnderscoreY = TextY + UnderscoreOffsetY;

  TextX = MapX(TextX);
  TextY = MapY(TextY);
  UnderscoreX = MapX(UnderscoreX);
  UnderscoreY = MapY(UnderscoreY);

  Screen->drawStr(TextX, TextY, Buffer);
  Screen->drawBox(UnderscoreX, UnderscoreY, UnderscoreWidth, UnderscoreHeight);
}

void StatePrinter::DisplayNotConnectedError()
{
  const uint8_t BufferSize = 16;
  char Buffer[BufferSize];

  strcpy(Buffer, "not conn");

  Screen->setFont(u8g2_font_profont22_tf);

  uint16_t TextWidth = Screen->getStrWidth(Buffer);
  uint16_t TextHeight = Screen->getAscent() + Screen->getDescent();

  uint16_t TextX = GetCenterX() - (TextWidth / 2);
  uint16_t TextY = GetCenterY() + (TextHeight / 2);

  Screen->drawStr(MapX(TextX), MapY(TextY), Buffer);
}

void StatePrinter::DisplayNoDistanceError()
{
  const uint8_t BufferSize = 16;
  char Buffer[BufferSize];

  strcpy(Buffer, "no dist");

  Screen->setFont(u8g2_font_profont22_tf);

  uint16_t TextWidth = Screen->getStrWidth(Buffer);
  uint16_t TextHeight = Screen->getAscent() + Screen->getDescent();

  uint16_t TextX = GetCenterX() - (TextWidth / 2);
  uint16_t TextY = GetCenterY() + (TextHeight / 2);

  Screen->drawStr(MapX(TextX), MapY(TextY), Buffer);
}

void StatePrinter::Display(me_SR04_StateGetter::State DataState)
{
  if (!DataState.IsConnected)
    DisplayNotConnectedError();
  else if (!DataState.HasDistance)
    DisplayNoDistanceError();
  else if (DataState.HasDistance)
    DisplayDistance(DataState.DistanceCm);
}
