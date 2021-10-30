// BMP280 barometer datalogger

/*
  Status: done
  Version: 1.1
  Last mod.: 2021-10-30
*/

/*
  Equipment and wiring:

    * Barometer BMP280, I2C;
    * Humidity meter DHT22;
    * LCD 20x4, I2C;
    * SD card FAT32, SPI;
    * RTC DS3231, I2C;
*/

#include <Adafruit_BMP280.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
#include "me_DataLogger.h"
#include "me_ds3231.h"
#include "me_DateTime.h"

const uint8_t 
  Sdcard_Cs_pin = 10,
  Dht22_pin = 2;

const uint32_t 
  MeasurementDelay_sec = 10;

Adafruit_BMP280 bmp;
SimpleDHT22 dht22(Dht22_pin);
LiquidCrystal_I2C lcd(0x27, 20, 4);
me_DataLogger dataLogger(Sdcard_Cs_pin);
me_ds3231 rtc = me_ds3231();

uint8_t deg[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000,
};

void setup() {
  Serial.begin(9600);
  // Serial.println(F("Barometer BMP280 data logger."));

  if (!bmp.begin()) {
    Serial.println(
      F("Could not find a valid BMP280 sensor, check wiring or "
        "try a different address!")
    );
    while (1)
      delay(10);
  }

  /*
    Default settings from datasheet.
      Operating Mode.
      Temp. oversampling.
      Pressure oversampling.
      Filtering.
      Standby time.
  */
  bmp.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X2,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, deg);

  // Serial.println("Init done.");
}

void loop() {
  float
    temperature = bmp.readTemperature(),
    pressure = bmp.readPressure(),
    temperature_dht,
    humidity;

  int16_t result = dht22.read2(&temperature_dht, &humidity, NULL);
  if (result != SimpleDHTErrSuccess) {
    temperature_dht = -128.0;
    humidity = -1.0;
  }

  DateTime dt = rtc.getDateTime();

  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(dt.representDateTime());

  lcd.setCursor(0, 1);
  lcd.print(temperature);
  lcd.print(" ");
  lcd.write(0);
  lcd.print("C ");

  lcd.setCursor(0, 2);
  lcd.print(pressure);
  lcd.print(" Pa ");

  lcd.setCursor(0, 3);
  lcd.print(temperature_dht);
  lcd.print(" ");
  lcd.write(0);
  lcd.print("C");
  lcd.print(" ");
  lcd.print(humidity);
  lcd.print(" %");

  String DataLoggerStr = "";
  DataLoggerStr += dt.representDateTime();
  DataLoggerStr += " " + String(temperature, 2);
  DataLoggerStr += " " + String(pressure, 2);
  DataLoggerStr += " " + String(temperature_dht, 2);
  DataLoggerStr += " " + String(humidity, 2);

  dataLogger.writeString(DataLoggerStr);
  Serial.println(DataLoggerStr);

  delay(MeasurementDelay_sec * 1000);
}

String representDate(DateTime dt) {
  String result = "";
  const uint8_t maxBufLen = 16;
  char buf[maxBufLen];
  dt.represent_date(buf, maxBufLen);
  result += buf;
  dt.represent_time(buf, maxBufLen);
  result += " " + String(buf);
  return result;
}
