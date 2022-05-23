#pragma once

#include <Arduino.h>

#include <me_IrNecParser.h>
#include <U8g2lib.h>
#include <me_StateGetters_IrNecParser.h>

void IrNec_DisplayState(me_IrNecParser_StateGetter::ParserState parserState, U8G2* Display);
