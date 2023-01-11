// Ultrasonic distance meter

/*
  Hardware used:

    Controller: ATmega328P
    Distance meter: HC-SR04
    Display: SH1106 128x64 I2C

  Wiring:

    TriggerPin: "Trig"
    EchoPin: "Echo"
*/

/*
  Version: 6
  Last mod.: 2023-01-10
*/

const uint8_t
  TriggerPin = 11,
  EchoPin = 12;

#include <U8g2lib.h>

#include <me_SR04.h>
#include <me_SR04_Distance.h>
#include <me_SR04_StatePrinter_128x64.h>

#include <me_CapacitiveFilter.h>
#include <me_Statistics_TimePoint.h>
#include <me_Statistics.Bucket.h>
#include <me_Stat_AverageFilter.h>

#include <me_Math_Scaling.h>

me_SR04::Sonar sonar(TriggerPin, EchoPin);
me_SR04_Distance::SensorDistance sensorDistance(&sonar);

U8G2_SH1106_128X64_NONAME_F_HW_I2C screen(U8G2_R0);
me_SR04_StatePrinter_128x64::StatePrinter _StatePrinter(&screen);

void setup()
{
  screen.begin();
  screen.setContrast(10);

  screen.clearBuffer();
  screen.drawFrame(0, 0, 128, 64);
  screen.sendBuffer();

  Serial.begin(57600);
}

void loop()
{
  const uint8_t
    MeasurementsPerCycle = 12; // 48

  const uint32_t
    CycleDurationMs = 800;  // 800

  const uint32_t
    MeasurementsDelayMs = CycleDurationMs / MeasurementsPerCycle;

  float minDistance = 1e6;
  float maxDistance = -1.0;

  uint8_t numNotConnectedErrors = 0;
  uint8_t numSuccessfullMeasurements = 0;

  for (uint8_t i = 0; i < MeasurementsPerCycle; ++i)
  {
    sensorDistance.Measure();

    bool result_NotConnected = sensorDistance.NotConnected();
    bool result_HasDistance = sensorDistance.HasDistance();
    float result_Distance = sensorDistance.DistanceCm();

    if (result_NotConnected)
      ++numNotConnectedErrors;

    if (result_HasDistance)
      ++numSuccessfullMeasurements;

    if (result_HasDistance)
    {
      minDistance = min(minDistance, result_Distance);
      maxDistance = max(maxDistance, result_Distance);
    }

    displayProgress(
      i + 1,
      MeasurementsPerCycle,
      result_NotConnected,
      result_HasDistance,
      result_Distance
    );

    delay(MeasurementsDelayMs);
  }

  bool isConnected = (numNotConnectedErrors < MeasurementsPerCycle);
  bool hasDistance = (numSuccessfullMeasurements > 0);

  screen.setDrawColor(0);
  screen.drawBox(1, 1, 126, 62);
  screen.setDrawColor(1);

  _StatePrinter.Display(isConnected, hasDistance, minDistance);
  screen.sendBuffer();
}

void displayProgress(
  uint8_t currentStep,
  uint8_t totalSteps,
  bool isNotConnected,
  bool hasDistance,
  float distanceCm
)
{
  /*
  Serial.println("Progress:");

  Serial.print("  Measurement: ");
  Serial.print(currentStep);
  Serial.print(" / ");
  Serial.print(totalSteps);
  Serial.println();

  Serial.print("  Is connected: ");
  Serial.print(!isNotConnected);
  Serial.println();

  Serial.print("  Has distance: ");
  Serial.print(hasDistance);
  Serial.println();

  Serial.print("  Distance (cm): ");
  Serial.print(distanceCm);
  Serial.println();

  Serial.println();
  */

  // Display progress as dots in ellipse with center in display (
  const uint8_t ellipseCenterX = 128 / 2 - 1;
  const uint8_t ellipseCenterY = 64 / 2 - 1;
  const uint8_t ellipseSizeX = 128 - 12;
  const uint8_t ellipseSizeY = 64 - 12;

  u8g2_uint_t x, y;

  float progress = (float) currentStep / totalSteps;
  progress = progress + ((float) random(100) / 100) * 0.6 / totalSteps;
  float angleRad = progress * TWO_PI - PI / 2;

  float cosA = cos(angleRad);
  float sinA = sin(angleRad);
  float powCosA = copysign(pow(abs(cosA), 1.75), cosA);
  float powSinA = copysign(pow(abs(sinA), 0.5), sinA);
  x = (float) (ellipseSizeX / 2) * powCosA + ellipseCenterX;
  y = (float) (ellipseSizeY / 2) * powSinA + ellipseCenterY;

  if (hasDistance)
    screen.drawDisc(x, y, 4);
  else
    screen.drawCircle(x, y, 4);

  // screen.drawLine(ellipseCenterX, ellipseCenterY, x, y);

  screen.sendBuffer();
  // )
}
