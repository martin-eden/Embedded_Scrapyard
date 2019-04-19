#include "me_ds3231.h"
#include "DateTime.h"
#include <LiquidCrystal.h>

const uint8_t
  rs = 12,
  en = 11,
  d4 = 5,
  d5 = 4,
  d6 = 3,
  d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

me_ds3231 ds3231 = me_ds3231();

void setup()
{
  // columns, rows
  lcd.begin(16, 2);
  Serial.begin(9600);

  ds3231.setSqwMode(0);
  ds3231.enableSqw();

  if (ds3231.wave32kEnabled())
  {
    Serial.println("32K output enabled. Disabling.");
    ds3231.disable32kWave();
  }

  if (ds3231.oscillatorWasStopped())
  {
    Serial.println("Clock was stopped. Battery is over?");
    ds3231.clearOscillatorWasStopped();
  }
  if (!ds3231.isOscillatorAtBattery())
  {
    Serial.println("Oscillator was disabled at battery mode. Enabling.");
    ds3231.enableOscillatorAtBattery();
  }

  /*ds3231.setDateTime(
    DateTime(F(__DATE__), F(__TIME__)) +
    TimeSpan(7)
  );*/
}

const uint8_t max_msg_len = 40;
char msg_buf[max_msg_len];

void do_business()
{
  DateTime dt = ds3231.getDateTime();

  dt.represent_date(msg_buf, max_msg_len);
  lcd.setCursor(0, 0);
  lcd.print(msg_buf);

  dt.represent_dow(msg_buf, max_msg_len);
  lcd.setCursor(11, 0);
  lcd.print(msg_buf);

  dt.represent_time(msg_buf, max_msg_len);
  lcd.setCursor(0, 1);
  lcd.print(msg_buf);
  // Serial.println(msg_buf);

  lcd.setCursor(9, 1);
  lcd.print(ds3231.getTemp(), 2);
  lcd.print("\337C");
  // Serial.print(ds3231.getTemp(), 2);
  // Serial.println("\337C");
}

void loop()
{
  do_business();
  delay(1000);
}
