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
  Display->drawVLine(66, 0, 32);
  Display->drawVLine(106, 0, 32);
}

void DisplayAddress(U8G2* Display, uint16_t Address)
{
  char Buffer[5];
  sprintf(Buffer, "%04X", Address);

  Display->setFont(u8g2_font_logisoso24_tr);
  Display->drawStr(0, 26, Buffer);
}

void DisplayCommand(U8G2* Display, uint8_t Command)
{
  char Buffer[3];
  sprintf(Buffer, "%02X", Command);

  Display->setFont(u8g2_font_logisoso24_tr);
  Display->drawStr(71, 26, Buffer);
}

void DisplayHasShortRepeat(U8G2* Display, bool HasShortRepeat)
{
  if (HasShortRepeat)
  {
    Display->setFont(u8g2_font_chargen_92_tr);
    Display->drawGlyph(107, 27, '.');
  }
}

void DisplayIsRepeat(U8G2* Display, bool IsRepeat)
{
  if (IsRepeat)
  {
    const uint16_t ReplaySymbolCode = 0x2b6e;
    Display->setFont(u8g2_font_unifont_t_86);
    Display->drawGlyph(113, 18, ReplaySymbolCode);
  }
}

void IrNec_DisplayState(IrNecParser::me_IrNecParser* IrNec, U8G2* Display)
{
  DisplayGridLines(Display);
  DisplayAddress(Display, IrNec->Address);
  DisplayCommand(Display, IrNec->Command);
  DisplayHasShortRepeat(Display, IrNec->HasShortRepeat);
  DisplayIsRepeat(Display, IrNec->IsRepeat);
}
