// Ultrasound distance meter HC-SR04 test.

/*
  Status: stable
  Version: 1.2
  Last mod.: 2022-07-02
*/

#include <me_SR04.h>
#include <me_SR04_StateGetter.h>
#include <me_SR04_StatePrinter_128x32.h>
#include <me_CapacitiveFilter.h>
#include <U8g2lib.h>

const uint8_t
  TriggerPin = 4,
  EchoPin = 4;

const uint32_t
  MeasurementDelayMs = 100;

const float
  DataFilterCapacitance = 4.0;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R2);

me_SR04::SR04 Sonar(TriggerPin, EchoPin);
CapacitiveFilter dataSmoother = CapacitiveFilter(DataFilterCapacitance);
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

  if (RequestStatus.HasDistance)
  {
    float distance = RequestStatus.DistanceCm;
    dataSmoother.addValue(distance);
    distance = dataSmoother.getValue();
    RequestStatus.DistanceCm = distance;
    Serial.println(RequestStatus.DistanceCm);
  }

  Display.clearBuffer();
  StatePrinter.Print(RequestStatus);
  Display.sendBuffer();

  delay(MeasurementDelayMs);
}
