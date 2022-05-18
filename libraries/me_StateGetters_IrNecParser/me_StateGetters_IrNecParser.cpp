#include "me_StateGetters_IrNecParser.h"
#include <me_IrNecParser.h>

void me_IrNecParser_StateGetter::GetState(me_IrNecParser::Parser* parser, ParserState& parserState)
{
  parserState.Address = parser->Address;
  parserState.Command = parser->Command;
  parserState.HasShortRepeat = parser->HasShortRepeat;
  parserState.IsRepeat = parser->IsRepeat;
}
