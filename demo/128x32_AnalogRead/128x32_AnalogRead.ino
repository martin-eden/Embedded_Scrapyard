#include <U8g2lib.h>
#include <me_ProgressBar.h>

const uint8_t AnalogPin = A0;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R0);
me_ProgressBar ProgressBar((U8G2*) &Display, 0, 0, 127, 31);

void setup()
{
  // pinMode(AnalogPin, INPUT_PULLUP);
  pinMode(AnalogPin, INPUT);
  Display.begin();
}

void loop()
{
  uint16_t AnalogValue = analogRead(AnalogPin);

  ProgressBar.CurrentValue = AnalogValue;

  Display.clearBuffer();

  ProgressBar.Draw();

  Display.sendBuffer();
}
