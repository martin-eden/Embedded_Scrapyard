#pragma once

#include <Arduino.h>

#include <me_IrNecParser.h>
#include <U8g2lib.h>

void IrNec_DisplayState(IrNecParser::me_IrNecParser* IrNec, U8G2* Display);
