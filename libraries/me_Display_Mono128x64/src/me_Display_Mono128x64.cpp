// 128x64 monochrome display wrapper over [U8g2] library

/*
  Author: Martin Eden
  Last mod.: 2024-09-22
*/

#include "me_Display_Mono128x64.h"

/*
  Both <me_BaseTypes.h> and <U8g2lib.h> are already included in ".h".
  So these are just a reminders for conceptual integrity.
*/
#include <U8g2lib.h>
#include <me_BaseTypes.h>

using namespace me_Display_Mono128x64;

/*
  Initialize code for display. Fill <this.Display>.

  Implementation is based on constructor's call
  "U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI".

  So display chip is SH1106, dimensions 128x64, communicate using
  hardware SPI facilities.
*/
void TDisplay_Mono128x64::Init(
  TUint_1 ChipSelect_Pin,
  TUint_1 DataCommand_Pin,
  TUint_1 Reset_Pin
)
{
  u8g2_t * DisplayStateAddress = Display.getU8g2();
  const u8g2_cb_t * NoRotation = &u8g2_cb_r0;
  u8x8_msg_cb Sender = u8x8_byte_arduino_hw_spi;
  u8x8_msg_cb Sleeper = u8x8_gpio_and_delay_arduino;

  u8g2_Setup_sh1106_128x64_noname_f(
    DisplayStateAddress,
    NoRotation,
    Sender,
    Sleeper
  );

  u8x8_t * BaseDisplayStateAddress = Display.getU8x8();

  u8x8_SetPin_4Wire_HW_SPI(
    BaseDisplayStateAddress,
    ChipSelect_Pin,
    DataCommand_Pin,
    Reset_Pin
  );
}

/*
  2024-09-22
*/
