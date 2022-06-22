// Ultrasound distance meter HC-SR04 test.

/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-06-20
*/

#include <me_SR04.h>
#include <me_SR04_StateGetter.h>
#include <me_SR04_StatePrinter_128x32.h>
#include <U8g2lib.h>

const uint8_t
  TriggerPin = 4,
  EchoPin = 4;

const uint32_t
  MeasurementDelayMs = 100;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R2);

me_SR04::SR04 Sonar(TriggerPin, EchoPin);
me_SR04_StatePrinter_128x32::StatePrinter StatePrinter(&Display);

void setup()
{
  Serial.begin(57600);

  Display.begin();
  Display.setContrast(10);
}

void loop()
{
  Sonar.Request();
  me_SR04_StateGetter::State RequestStatus = me_SR04_StateGetter::GetState(&Sonar);

  Display.clearBuffer();
  StatePrinter.Print(RequestStatus);
  Display.sendBuffer();

  if (RequestStatus.HasDistance)
  {
    Serial.println(RequestStatus.DistanceCm);
  }

  delay(MeasurementDelayMs);
}
