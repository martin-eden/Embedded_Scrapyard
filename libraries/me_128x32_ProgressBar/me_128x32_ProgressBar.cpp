// Progress bar for monochrome OLED display.

/*
  Status: working prototype
  Version: 0.1
  Last mod.: 2022-02-18
*/

#include "me_128x32_ProgressBar.h"

me_128x32_ProgressBar::me_128x32_ProgressBar(
  U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C* aDisplay,
  uint8_t aLeft = 0,
  uint8_t aTop = 0,
  uint8_t aRight = 127,
  uint8_t aBottom = 31
)
{
  Display = aDisplay;
  Left = aLeft;
  Top = aTop;
  Right = aRight;
  Bottom = aBottom;
}

void me_128x32_ProgressBar::Draw()
{
  DrawOuterBox();
  DrawFill();
}

void me_128x32_ProgressBar::DrawOuterBox()
{
  Display->setDrawColor(1);
  Display->drawFrame(Left, Top, Right - Left + 1, Bottom - Top + 1);
  Display->setDrawColor(0);
  Display->drawBox(Left + 1, Top + 1, Right - 1 - Left - 1 + 1, Bottom - 1 - Top - 1 + 1);
}

void me_128x32_ProgressBar::DrawFill()
{
  CurrentValue = constrain(CurrentValue, MinValue, MaxValue);
  uint8_t AwailableWidth = Right - Left - 3;
  uint8_t OccupiedWidth = (float) AwailableWidth * (((float)CurrentValue - MinValue) / MaxValue);
  Display->setDrawColor(1);
  Display->drawBox(Left + 2, Top + 2, OccupiedWidth, Bottom - Top - 3);
}
