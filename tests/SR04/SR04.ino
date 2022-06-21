// Ultrasound distance meter HC-SR04 test.

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-06-20
*/

#include <me_SR04.h>

const uint8_t
  TriggerPin = 6,
  EchoPin = 7;

const uint32_t
  MeasurementDelayMs = 10;

me_SR04::SR04 Sonar(TriggerPin, EchoPin);

void setup()
{
  Serial.begin(57600);
}

void loop()
{
  Sonar.Request();
  if (Sonar.RequestStatus == me_SR04::ReadStatus::Error)
  {
    // Serial.println("Timeout.");
  }
  else
  {
    // Serial.print(Sonar.EchoDelayMcr);
    // Serial.print(" ");
    // Serial.print(Sonar.EchoDurationMcr);
    // Serial.println();
    float distanceCm = Sonar.EchoDurationMcr * 1e-6 * 340.0 * 1e2 / 2;
    Serial.println(distanceCm);
  }
  delay(MeasurementDelayMs);
}
