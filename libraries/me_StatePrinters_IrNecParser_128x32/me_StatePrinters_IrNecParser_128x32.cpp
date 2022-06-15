// Custom display IR NEC data.

/*
  Status: stable
  Version: 1.2
  Last mod.: 2022-06-15
*/

#include "me_StatePrinters_IrNecParser_128x32.h"

#include <me_IrNecParser.h>
#include <U8g2lib.h>

void DisplayOuterFrame(U8G2* Display)
{
  Display->drawFrame(0, 0, 128, 32);
}

void DisplayGridLines(U8G2* Display)
{
  Display->drawVLine(68, 4, 25);
  Display->drawVLine(106, 4, 25);
}

const u8g2_uint_t
  AddressWidgetX = 3,
  AddressWidgetY = 26;

void DisplayAddress(U8G2* Display, uint16_t Address)
{
  char Buffer[5];
  sprintf(Buffer, "%04X", Address);

  Display->setFont(u8g2_font_profont29_tr);
  Display->drawStr(AddressWidgetX, AddressWidgetY, Buffer);
}

const u8g2_uint_t
  CommandWidgetX = 72,
  CommandWidgetY = 26;

void DisplayCommand(U8G2* Display, uint8_t Command)
{
  char Buffer[3];
  sprintf(Buffer, "%02X", Command);

  Display->setFont(u8g2_font_profont29_tr);
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
  RepeatWidgetY = 21;

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
  FlipFlopWidgetX1 = 112,
  FlipFlopWidgetX2 = 120,
  FlipFlopWidgetY = 7;

void DisplayFlipFlop(U8G2* Display)
{
  static uint8_t FlipState = 0;

  u8g2_uint_t x, y;
  switch (FlipState)
  {
    case 0:
      x = FlipFlopWidgetX1;
      y = FlipFlopWidgetY;
      break;
    case 1:
      x = FlipFlopWidgetX2;
      y = FlipFlopWidgetY;
      break;
    default:
      exit(1);
  }

  Display->drawDisc(x, y, 2);

  FlipState = (FlipState + 1) % 2;
}

void IrNec_DisplayState(me_IrNecParser_StateGetter::ParserState parserState, U8G2* Display)
{
  DisplayOuterFrame(Display);
  DisplayGridLines(Display);
  DisplayAddress(Display, parserState.Address);
  DisplayCommand(Display, parserState.Command);
  DisplayHasShortRepeat(Display, parserState.HasShortRepeat);
  DisplayIsRepeat(Display, parserState.IsRepeat);
  DisplayFlipFlop(Display);
}
