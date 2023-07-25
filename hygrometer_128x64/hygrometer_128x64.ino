// Display humidity and temperature reading on 128x64 display.

/*
  Status: stable
  Version: 2
  Last mod.: 2023-07-25
*/

/*
  Equipment and wiring.

    Hygrometer: DHT11

      V
      <Hygrometer_pin>
      -
      G

    Display: 128x64 "1.3 OLED, <SPI 4W>, SH1106

      G
      V
      CLK 13
      MOS 11
      RES <Display_reset_pin>
      DC <Display_dc_pin>
      CS <Display_cs_pin>
*/

/*
  This code is forked from [hygrometer_128x32] 2023-03-30.
  Intention is to adjust display to 128x64 SPI 4-wire.
*/

#include <U8g2lib.h>

#include <me_DHT11.h>
#include <me_DHT11_StateGetter.h>
#include <me_DHT11_StatePrinter_128x64.h>

#define Display_cs_pin 8
#define Display_dc_pin 9
#define Display_reset_pin 10

const uint8_t
  Hygrometer_pin = 2;

const uint32_t
  BootDelay_ms = 300,
  MeasurementDelay_ms = 6000;

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI Display(U8G2_R0, Display_cs_pin, Display_dc_pin, Display_reset_pin);

me_DHT11::DHT11 Hygrometer(Hygrometer_pin);
me_DHT11_StatePrinter_128x64::StatePrinter StatePrinter(&Display);

void setup()
{
  Display.begin();
  delay(BootDelay_ms);
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
