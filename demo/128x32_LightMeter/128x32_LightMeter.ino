#include <U8g2lib.h>
#include <me_ProgressBar.h>
#include <me_RandomDots.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R0);
me_ProgressBar ProgressBar((U8G2*) &Display, 8, 6, 119, 25);
me_RandomDots RandomDots((U8G2*) &Display, 0, 0, 127, 31);

void setup()
{
  Display.begin();
}

void loop()
{
  uint16_t lighting = 1023 - analogRead(A0);

  RandomDots.CurrentFillRatio = lighting;
  ProgressBar.CurrentValue = lighting;

  Display.clearBuffer();

  RandomDots.Draw();
  ProgressBar.Draw();

  Display.sendBuffer();

  delay(250);
}
