// Capturing and parsing IR signals in NEC format.

/*
  Status: stable
  Version: 1.2
  Last mod.: 2022-04-02
*/

#include <me_DigitalSignalRecorder.h>
#include <me_IrNecParser.h>
#include <me_StatePrinters_IrNecParser_128x32.h>
#include <U8g2lib.h>

using namespace IrNecParser;

me_DigitalSignalRecorder DSR;
me_IrNecParser IrDecoder(&DSR);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R2);

const uint8_t
  SignalPin = 2; // 2 or 3 for ATmega328P

const uint16_t
  RecorderCapacity = 38;

void setup()
{
  Serial.begin(115200);
  Serial.println("IR NEC protocol decoder test.\nDisplays results on 128x32 OLED display.");

  Display.begin();
  Display.setContrast(10);

  if (!DSR.SetCapacity(RecorderCapacity))
  {
    Serial.println("Not enough memory for recorder's buffer.");
    delay(1000);
    exit(1);
  }

  pinMode(SignalPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(SignalPin), OnSignalChange, CHANGE);
}

void OnSignalChange()
{
  DSR.Add(micros(), digitalRead(SignalPin));
}

void loop()
{
  while (IrDecoder.Get())
  {
    Display.clearBuffer();
    IrNec_DisplayState(&IrDecoder, &Display);
    Display.sendBuffer();
  }

  delay(50);
}
