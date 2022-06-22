// Ultrasound distance meter HC-SR04 test.

/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-06-20
*/

#include <me_SR04.h>
#include <me_SR04_StateGetter.h>

const uint8_t
  TriggerPin = 4,
  EchoPin = 4;

const uint32_t
  MeasurementDelayMs = 60;

me_SR04::SR04 Sonar(TriggerPin, EchoPin);

void setup()
{
  Serial.begin(57600);
}

void loop()
{
  Sonar.Request();
  me_SR04_StateGetter::State RequestStatus = me_SR04_StateGetter::GetState(&Sonar);
  if (RequestStatus.HasDistance)
  {
    Serial.println(RequestStatus.DistanceCm);
  }
  delay(MeasurementDelayMs);
}
