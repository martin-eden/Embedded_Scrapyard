// Thermostat with OLED display.

/*
  Status: stable
  Last mod.: 2021-01-12
  Version: 1.1.0
*/

/*
  Built on:
    Dallas 18B20 thermometer
    Relay module
    OLED 128x64 SH1106
    Arduino Uno

  Also it reads temperature from DS3231 and if it below <MIN_TEMP_ON> turns
  <SWITCH_PIN> on. If it above <MAX_TEMP_OFF> - turns switch off.

  Wiring.
    OLED
      CLK = 13
      MOS = 11
      Other wiring adjustable in <OLED_*> constants.

    18B20
      <TEMP_PIN>

    Relay
      <SWITCH_PIN>

  Adjust <DESIRED_TEMP_..> to required temperature band.
*/

#include <U8x8lib.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <me_ds3231.h>
#include <me_switch.h>
#include <me_DateTime.h>
#include <me_Thermostat.h>
#include <me_CapacitiveFilter.h>

const uint8_t
  OLED_RES_PIN = 8,
  OLED_DC_PIN = 9,
  OLED_CS_PIN = 10,

  TEMP_PIN = 2,

  SWITCH_PIN = 4;

const float
  MIN_TEMP_ON = 22.50,
  MAX_TEMP_OFF = 23.25;

const bool
  THERMOSTAT_ENABLING_INCREASES_VALUE = true;

const float
  DATA_FILTER_CAPACITANCE = 1.25;

U8X8_SH1106_128X64_NONAME_4W_HW_SPI oled(OLED_CS_PIN, OLED_DC_PIN, OLED_RES_PIN);
OneWire one_wire(TEMP_PIN);
DallasTemperature thermometer(&one_wire);
c_switch relay(SWITCH_PIN);
cThermostat thermostat(relay);
CapacitiveFilter capacitiveFilter = CapacitiveFilter(DATA_FILTER_CAPACITANCE);

void announce(const char* msg) {
  Serial.print("  ");
  Serial.println(msg);
}

void init_oled() {
  announce("OLED");
  oled.begin();
  oled.setFont(u8x8_font_profont29_2x3_n);
  oled.setPowerSave(0);
}

void init_thermometer() {
  announce("Thermometer");
  thermometer.begin();
}

void init_relay() {
  announce("Relay");
  pinMode(SWITCH_PIN, OUTPUT);
}

void init_thermostat() {
  announce("Thermostat");
  thermostat.min_value = MIN_TEMP_ON;
  thermostat.max_value = MAX_TEMP_OFF;
  thermostat.enabling_increases_value = THERMOSTAT_ENABLING_INCREASES_VALUE;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");

  init_oled();
  init_thermometer();
  init_relay();
  init_thermostat();

  Serial.println("Initialization done.");
}

const float
  DISCONNECTED_TEMP = -127.0;

void do_business() {
  static float prevTemp = 0.0;

  thermometer.requestTemperatures();
  float temperature = thermometer.getTempCByIndex(0);
  capacitiveFilter.addValue(temperature);
  temperature = capacitiveFilter.getValue();

  if (temperature != prevTemp) {
    String value_str = String(temperature, 3); // + "\260C";
    char buf_str[10];
    value_str.toCharArray(buf_str, 10);

    oled.drawString(2, 4, "      ");
    oled.drawString(2, 4, buf_str);
  }

  if (temperature != DISCONNECTED_TEMP) {
    thermostat.process(temperature);
  }

  prevTemp = temperature;
}

void loop() {
  do_business();
  delay(100);
}
