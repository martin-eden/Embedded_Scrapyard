// [me_Display_Mono128x64] basic test

/*
  Author: Martin Eden
  Last mod.: 2024-09-22
*/

#include <me_BaseTypes.h>
#include <me_InstallStandardStreams.h>
#include <me_UartSpeeds.h>

#include <me_Display_Mono128x64.h>

// 128x64 monochrome SH1106 SPI display wiring:
TUint_1 Display_ChipSelect_Pin = 8;
TUint_1 Display_DataCommand_Pin = 9;
TUint_1 Display_Reset_Pin = 10;


void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  InstallStandardStreams();

  printf("[me_Display_Mono128x64] Okay, we are here.\n");
  RunTest();
  printf("[me_Display_Mono128x64] Done.\n");
}

void loop()
{
}

// --

// Just display something to check wiring and orientation
void RunTest()
{
  me_Display_Mono128x64::TDisplay_Mono128x64 DisplayWrapper;

  DisplayWrapper.Init(
    Display_ChipSelect_Pin,
    Display_DataCommand_Pin,
    Display_Reset_Pin
  );

  U8G2 * Display = (U8G2 *) &DisplayWrapper.Display;

  Display->begin();

  // Draw frame touching borders
  {
    TUint_2 Width = Display->getDisplayWidth();
    TUint_2 Height = Display->getDisplayHeight();
    TUint_2 StartX = 0;
    TUint_2 StartY = 0;

    Display->drawFrame(StartX, StartY, Width, Height);
  }

  // Draw line from top left to middle
  {
    TUint_2 StartX = 0;
    TUint_2 StartY = 0;
    TUint_2 EndX = (Display->getDisplayWidth() - StartX) / 2;
    TUint_2 EndY = (Display->getDisplayHeight() - StartY) / 2;

    Display->drawLine(StartX, StartY, EndX, EndY);
  }

  Display->sendBuffer();
}

/*
  2024-09-22
*/
