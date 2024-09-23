// 128x64 monochrome display wrapper over [U8g2] stock library

/*
  Author: Martin Eden
  Last mod.: 2024-09-22
*/

/*
  Design

    This is a custom wrapper over [U8g2] library for specific
    display type.

    Base library exports zillion descendant classes for all displays
    it supports. Each descendant is just constructor with custom
    hardware setup code.

    This class calls hardware setup code for my fixed display and
    exports "U8G2" base class under field "Display".

    Instance of this class will consume a lot of memory.
    It's possible to use heap memory but I don't see use case for it.
    Typically Arduino has memory only for one display and can't afford
    memory-hungry functions.

    If you have memory hungry-functions, use this class in C++
    lexical scope.
*/

# pragma once

#include <U8g2lib.h>
#include <me_BaseTypes.h>

namespace me_Display_Mono128x64
{
  class TDisplay_Mono128x64
  {
    public:
      // [U8g2lib] display class. Valid after Init().
      U8G2 Display;

      // Setup library and initialize <this.Display>
      void Init(
        TUint_1 ChipSelect_Pin,
        TUint_1 DataCommand_Pin,
        TUint_1 Reset_Pin
      );
  };
}

/*
  2024-09-22
*/
