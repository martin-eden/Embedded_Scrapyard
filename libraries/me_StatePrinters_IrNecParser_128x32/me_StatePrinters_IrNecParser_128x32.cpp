// Custom display IR NEC data.

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-04-09
*/

#include "me_StatePrinters_IrNecParser_128x32.h"

#include <me_IrNecParser.h>
#include <U8g2lib.h>

void DisplayAddress(U8G2* Display, uint16_t Address)
{
  char Buffer[7];
  Display->setFont(u8g2_font_amstrad_cpc_extended_8r);
  sprintf(Buffer, "0x%04X", Address);
  Display->drawStr(0, 8, Buffer);
}

void DisplayCommand(U8G2* Display, uint8_t Command)
{
  char Buffer[5];
  Display->setFont(u8g2_font_chargen_92_tr);
  sprintf(Buffer, "0x%02X", Command);
  Display->drawStr(24, 26, Buffer);
}

void DisplayHasShortRepeat(U8G2* Display, bool HasShortRepeat)
{
  if (HasShortRepeat)
  {
    Display->setFont(u8g2_font_chargen_92_tr);
    Display->drawGlyph(70, 26, '.');
  }
}

void DisplayNumRepeats(U8G2* Display, uint16_t NumRepeats)
{
  if (NumRepeats != 0)
  {
    char Buffer[8];
    Display->setFont(u8g2_font_chargen_92_tr);
    Display->drawGlyph(90, 22, 'x');
    NumRepeats++;
    if (NumRepeats <= 99)
      sprintf(Buffer, "%d", NumRepeats);
    else
      sprintf(Buffer, "%s", "??");
    Display->drawStr(104, 24, Buffer);
  }
}

void IrNec_DisplayState(IrNecParser::me_IrNecParser* IrNec, U8G2* Display)
{
  DisplayAddress(Display, IrNec->Address);
  DisplayCommand(Display, IrNec->Command);
  DisplayHasShortRepeat(Display, IrNec->HasShortRepeat);
  DisplayNumRepeats(Display, IrNec->NumRepeats);
}
