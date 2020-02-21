// Table clock (with optional thermostat)

/*
  Status: testing
  Last mod.: 2020-02-11
  Version: 1.4.0
*/

/*
  Built on:
    16x2 LCD display
    DS3231 RTC
    relay module
    Dallas 18B20 thermometer
    Arduino Uno

  It sets DS3231 to emit square wave at 1Hz, connects output to
  interrupt pin and use it to update display. Also it reads
  temperature from DS3231 and if it below <MIN_TEMP_ON> turns
  <SWITCH_PIN> on. If it above <MAX_TEMP_OFF> - turns switch off.

  Wiring.
    Connect LCD to <LCD_..> pins.
    Connect DS3231.BBSQW to <TICK_PIN> pin which accepts interrupts.
    Connect relay to <SWITCH_PIN>.
    Connect 18B20 to <TEMP_PIN>.
    Adjust <DESIRED_TEMP_..> to required temperature band.
*/

#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <me_ds3231.h>
#include <me_switch.h>
#include <me_DateTime.h>
#include <me_Thermostat.h>

const uint8_t
  LCD_RS = 7,
  LCD_EN = 8,
  LCD_D4 = 9,
  LCD_D5 = 10,
  LCD_D6 = 11,
  LCD_D7 = 12,

  TEMP_PIN = 2,

  TICK_PIN = 3,

  SWITCH_PIN = 4;

const float
  MIN_TEMP_ON = 22.75,
  MAX_TEMP_OFF = 23.25;

const bool
  THERMOSTAT_ENABLING_INCREASES_VALUE = true;

const uint8_t
  MAX_MSG_LEN = 2 * 16;
char msg_buf[MAX_MSG_LEN];

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
OneWire one_wire(TEMP_PIN);
DallasTemperature sensors(&one_wire);
me_ds3231 ds3231 = me_ds3231();
c_switch relay(SWITCH_PIN);
cThermostat thermostat(relay);

void init_thermostat()
{
  Serial.println("thermostat.init");
  thermostat.min_value = MIN_TEMP_ON;
  thermostat.max_value = MAX_TEMP_OFF;
  thermostat.enabling_increases_value = THERMOSTAT_ENABLING_INCREASES_VALUE;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello.");

  lcd.begin(16, 2);

  sensors.begin();

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

  pinMode(SWITCH_PIN, OUTPUT);

  init_thermostat();
}

const float
  DISCONNECTED_TEMP = -127.0;

void do_business()
{
  DateTime dt = ds3231.getDateTime();
  float temperature;

  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

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
  lcd.print(temperature, 2);
  lcd.print("\337C");

  if (temperature != DISCONNECTED_TEMP) {
    thermostat.process(temperature);
  }
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

/*
  2019-12
  2020-01
  2020-02 added MAX_HEAT_SECS logic
*/
