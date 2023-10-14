// Ultrasonic distance display for 128x64 monochrome OLED.

/*
  Version: 3
  Last mod.: 2023-10-12
*/

#include "me_SR04_StatePrinter_128x64.h"

#include <me_SR04_Distance.h>
#include <me_SR04.h>

using namespace me_SR04_StatePrinter_128x64;

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

  char Buffer[BufferSize];

  DistanceInt = DistanceCm;

  snprintf(Buffer, BufferSize, "%d", DistanceInt);

  Screen->setFont(u8g2_font_profont29_tf);

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

void StatePrinter::Display(bool isConnected, bool hasDistance, float distanceCm)
{
  if (!isConnected)
  {
    DisplayNotConnectedError();
    return;
  }

  if (!hasDistance)
  {
    DisplayNoDistanceError();
    return;
  }

  DisplayDistance(distanceCm);
}

/*
  2023-01-09
*/
