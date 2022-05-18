#pragma once

#include <Arduino.h>

#include <me_IrNecParser.h>

namespace me_IrNecParser_StateGetter
{
  struct ParserState {
    uint16_t Address;
    uint8_t Command;
    bool HasShortRepeat;
    bool IsRepeat;
  };

  void GetState(me_IrNecParser::Parser* parser, ParserState& parserState);
}
