// Ultrasonic distance meter.

/*
  Version: 7
  Last mod.: 2023-06-04
*/

/*
  Hardware used

    Distance sensor: HC-SR04
    Controller: ATmega328P
    Display: SH1106 128x64 I2C

  Wiring

    "Trig" == TriggerPin
    "Echo" == EchoPin

    You can wire Trig and Echo to one pin.
*/

// Wiring:
const uint8_t
  TriggerPin = 11,
  EchoPin = 12;

// Screen settings:
const uint8_t
  ScreenContrast = 90;

// Functionality settings:
const uint8_t MeasurementsPerCycle = 16;
const uint32_t MeasurementsDelayMs = 10;

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
  screen.setContrast(ScreenContrast);

  screen.clearBuffer();
  screen.drawFrame(0, 0, 128, 64);
  screen.sendBuffer();
}

void loop()
{
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
