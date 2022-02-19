// Barometer and hygrometer with data logging and LCD display.

/*
  Status: stable
  Version: 1.3
  Last mod.: 2022-02-01
*/

/*
  Equipment, interface and wiring:

    * RTC DS3231, I2C;
    * Barometer BMP280, I2C;
    * Hygrometer DHT22, <Hygrometer_pin>;
    * LCD 20x4, I2C;
    * SD card (FAT32), SPI, <Sdcard_Cs_pin>;
*/

#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include <me_DHT22.h>
#include <me_DataLogger.h>
#include <me_ds3231.h>
#include <me_DateTime.h>

const uint8_t
  Sdcard_Cs_pin = 10,
  Hygrometer_pin = 2;

const uint32_t
  MeasurementDelay_sec = 15;

Adafruit_BMP280 Barometer;
me_DHT22 Hygrometer(Hygrometer_pin);
LiquidCrystal_I2C LCD(0x27, 20, 4);
me_DataLogger DataLogger(Sdcard_Cs_pin);
me_ds3231 RTC = me_ds3231();

const uint8_t degreeSignData[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000,
};
const uint8_t degreeSignCode = 0;

void setup() {
  Serial.begin(9600);
  // Serial.println(F("Barometer BMP280 data logger."));

  if (!Barometer.begin()) {
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
      barometerPressure oversampling.
      Filtering.
      Standby time.
  */
  Barometer.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X2,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );

  LCD.init();
  LCD.backlight();
  LCD.createChar(0, degreeSignData);

  // Serial.println("Init done.");
}

void loop() {
  float
    barometerTemperature = Barometer.readTemperature(),
    barometerPressure = Barometer.readPressure(),
    hygrometerTemperature,
    hygrometerHumidity;

  Hygrometer.Get();
  hygrometerTemperature = Hygrometer.Temperature;
  hygrometerHumidity = Hygrometer.Humidity;

  DateTime dt = RTC.getDateTime();

  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print(dt.representDateTime());

  LCD.setCursor(0, 1);
  LCD.print("Pa");
  LCD.print(" ");
  LCD.print((int32_t) barometerPressure);

  LCD.print(" ");
  LCD.write(degreeSignCode);
  LCD.print("C");
  LCD.print(" ");
  LCD.print(barometerTemperature);

  LCD.setCursor(0, 2);
  LCD.print("RH%");
  LCD.print(" ");
  LCD.print(hygrometerHumidity);
  LCD.print(" ");
  LCD.write(degreeSignCode);
  LCD.print("C");
  LCD.print(" ");
  LCD.print(hygrometerTemperature);

  String DataLoggerStr = "";
  DataLoggerStr += dt.representDateTime();
  DataLoggerStr += "\t" + String(barometerTemperature, 2);
  DataLoggerStr += "\t" + String(barometerPressure, 2);
  DataLoggerStr += "\t" + String(hygrometerTemperature, 2);
  DataLoggerStr += "\t" + String(hygrometerHumidity, 2);

  bool sdStatus = DataLogger.writeString(DataLoggerStr);
  LCD.setCursor(0, 3);
  if (sdStatus)
    LCD.print("[SD OK]");
  else
    LCD.print("[no SD?]");

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
