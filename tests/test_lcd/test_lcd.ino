#include <LiquidCrystal.h>

const uint8_t
  rs = 12,
  en = 11,
  d4 = 5,
  d5 = 4,
  d6 = 3,
  d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  lcd.begin(16, 2);
}

void loop()
{
  lcd.noDisplay();

  delay(500);

  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("0123456789ABCDEF");
  lcd.setCursor(0, 1);
  lcd.print("0123456789ABCDEF");

  delay(500);
}
