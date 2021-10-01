// BMP280 barometer test

/*
  Status: works
  Version: 0.9
  Last mod.: 2021-09-29
*/

/*
  Equipment and wiring.

  LCD 20x4 I2C
  BMP280 connected via I2C.
*/

#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>

Adafruit_BMP280 bmp;
LiquidCrystal_I2C lcd(0x27, 20, 4);

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
  Serial.println(F("BMP280 test"));

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
}

void loop() {
  float 
    temperature = bmp.readTemperature(),
    pressure = bmp.readPressure();

  lcd.setCursor(0, 0);
  lcd.print(temperature);
  lcd.write(0);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print(pressure);
  lcd.print(" Pa");

  delay(2000);
}
