// Table clock

/*
  Built on: 16x2 LCD display, DS3231 RTC, Arduino Uno

  It sets DS3231 to emit square wave at 1Hz, connects output to
  interrupt pin and use it to update display.

  Wiring.
    Connect LCD to <LCD_..> pins.
    Connect DS3231.BBSQW to <TICK_PIN> pin which accepts interrupts.
*/

/*
  Status: stable, extending
  Last mod.: 2019-12-16
  Version: 1.0.0
*/

#include "me_ds3231.h"
#include "DateTime.h"
#include <LiquidCrystal.h>

const uint8_t
  LCD_RS = 7,
  LCD_EN = 8,
  LCD_D4 = 9,
  LCD_D5 = 10,
  LCD_D6 = 11,
  LCD_D7 = 12,

  TICK_PIN = 3;

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
me_ds3231 ds3231 = me_ds3231();

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);

  // SQW mode 0 - 1Hz square wave
  ds3231.setSqwMode(0);
  ds3231.disableSqwAtBattery();

  if (ds3231.wave32kEnabled())
  {
    Serial.println("32K output enabled. Disabling.");
    ds3231.disable32kWave();
  }

  if (ds3231.oscillatorWasStopped())
  {
    Serial.println("Clock was stopped. Battery is over?");
  }

  if (!ds3231.isOscillatorAtBattery())
  {
    Serial.println("Oscillator was disabled at battery mode. Enabling.");
    ds3231.enableOscillatorAtBattery();
  }

  pinMode(TICK_PIN, INPUT_PULLUP);
  uint8_t internal_pin_no = digitalPinToInterrupt(TICK_PIN);
  attachInterrupt(internal_pin_no, tick_handler, RISING);
}

const uint8_t MAX_MSG_LEN = 40;
char msg_buf[MAX_MSG_LEN];

void do_business()
{
  DateTime dt = ds3231.getDateTime();

  dt.represent_date(msg_buf, MAX_MSG_LEN);
  lcd.setCursor(0, 0);
  lcd.print(msg_buf);

  dt.represent_dow(msg_buf, MAX_MSG_LEN);
  lcd.setCursor(11, 0);
  lcd.print(msg_buf);

  dt.represent_time(msg_buf, MAX_MSG_LEN);
  lcd.setCursor(0, 1);
  lcd.print(msg_buf);

  lcd.setCursor(9, 1);
  lcd.print(ds3231.getTemp(), 2);
  lcd.print("\337C");
}

volatile bool tick_registered;

void loop()
{
  if (tick_registered)
  {
    do_business();
    tick_registered = false;
  }
}

void tick_handler()
{
  // Serial.println("beep");
  tick_registered = true;
}
