// Custom display IR NEC data.

/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-04-16
*/

#include "me_StatePrinters_IrNecParser_128x32.h"

#include <me_IrNecParser.h>
#include <U8g2lib.h>

void DisplayGridLines(U8G2* Display)
{
  Display->drawFrame(0, 0, 128, 32);
  Display->drawVLine(68, 4, 25);
  Display->drawVLine(106, 4, 25);
}

const u8g2_uint_t
  AddressWidgetX = 1,
  AddressWidgetY = 26;

void DisplayAddress(U8G2* Display, uint16_t Address)
{
  char Buffer[5];
  sprintf(Buffer, "%04X", Address);

  Display->setFont(u8g2_font_osr18_tr);
  Display->drawStr(AddressWidgetX, AddressWidgetY, Buffer);
}

const u8g2_uint_t
  CommandWidgetX = 71,
  CommandWidgetY = 26;

void DisplayCommand(U8G2* Display, uint8_t Command)
{
  char Buffer[3];
  sprintf(Buffer, "%02X", Command);

  Display->setFont(u8g2_font_osr18_tr);
  Display->drawStr(CommandWidgetX, CommandWidgetY, Buffer);
}

const u8g2_uint_t
  ShortRepeatWidgetX = 109,
  ShortRepeatWidgetY = 27;

void DisplayHasShortRepeat(U8G2* Display, bool HasShortRepeat)
{
  if (HasShortRepeat)
  {
    Display->setFont(u8g2_font_osr18_tr);
    Display->drawGlyph(ShortRepeatWidgetX, ShortRepeatWidgetY, '.');
  }
}

const u8g2_uint_t
  RepeatWidgetX = 110,
  RepeatWidgetY = 20;

void DisplayIsRepeat(U8G2* Display, bool IsRepeat)
{
  if (IsRepeat)
  {
    const uint16_t ReplaySymbolCode = 0x2b6e;
    Display->setFont(u8g2_font_unifont_t_86);
    Display->drawGlyph(RepeatWidgetX, RepeatWidgetY, ReplaySymbolCode);
  }
}

const u8g2_uint_t
  FlipFlopWidgetX = 114,
  FlipFlopWidgetY = 25;

void DisplayFlipFlop(U8G2* Display)
{
  static uint8_t FlipState = 0;

  u8g2_uint_t x, y;
  switch (FlipState)
  {
    case 0:
      x = 0;
      y = 0;
      break;
    case 1:
      x = 127;
      y = 0;
      break;
    case 2:
      x = 127;
      y = 31;
      break;
    case 3:
      x = 0;
      y = 31;
      break;
    default:
      exit(1);
  }

  Display->drawDisc(x, y, 3);

  FlipState = (FlipState + 1) % 4;
}

void IrNec_DisplayState(IrNecParser::me_IrNecParser* IrNec, U8G2* Display)
{
  DisplayGridLines(Display);
  DisplayAddress(Display, IrNec->Address);
  DisplayCommand(Display, IrNec->Command);
  DisplayHasShortRepeat(Display, IrNec->HasShortRepeat);
  DisplayIsRepeat(Display, IrNec->IsRepeat);
  DisplayFlipFlop(Display);
}
