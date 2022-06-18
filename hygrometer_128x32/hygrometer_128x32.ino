// Display humidity and temperature reading on 128x32 display.

/*
  Status: stable
  Version: 3.0
  Last mod.: 2022-06-18
*/

/*
  Equipment and wiring

    Display: 128x32 ".9 OLED, <I2C>
    Hygrometer: DHT11, <Hygrometer_pin>
*/

#include <U8g2lib.h>

#include <me_DHT11.h>
#include <me_DHT11_StateGetter.h>
#include <me_DHT11_StatePrinter_128x32.h>

const uint8_t
  Hygrometer_pin = 2;

const uint32_t
  MeasurementDelay_ms = 6000;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R0);

me_DHT11::DHT11 Hygrometer(Hygrometer_pin);
me_DHT11_StatePrinter_128x32::StatePrinter StatePrinter(&Display);

void setup()
{
  Display.begin();
}

void loop()
{
  me_DHT11_StateGetter::State DataState;

  Hygrometer.Request();
  DataState = me_DHT11_StateGetter::GetState(&Hygrometer);

  Display.clearBuffer();
  StatePrinter.Print(DataState);
  Display.sendBuffer();

  delay(MeasurementDelay_ms);
}
