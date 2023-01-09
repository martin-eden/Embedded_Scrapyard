// Graphical ruler, 128x64 mono. Uses <me_SR04.h>

/*
  Version: 4
  Last mod.: 2022-12-03
*/

#include <U8g2lib.h>

#include <me_SR04.h>
#include <me_SR04_Distance.h>
#include <me_SR04_StatePrinter_128x64.h>

#include <me_CapacitiveFilter.h>
#include <me_Statistics_TimePoint.h>
#include <me_Statistics.Bucket.h>
#include <me_Stat_AverageFilter.h>

#include <me_Math_Scaling.h>

const uint8_t
  TriggerPin = 11,
  EchoPin = 12;

const uint8_t
  MeasurementsPerCycle = 48;

const uint32_t
  CycleDurationMs = 800;

const uint32_t
  MeasurementsDelayMs = CycleDurationMs / MeasurementsPerCycle;

me_SR04::Sonar _Sonar(TriggerPin, EchoPin);
me_SR04_Distance::SensorDistance _SensorDistance(&_Sonar);

U8G2_SH1106_128X64_NONAME_F_HW_I2C Display(U8G2_R0);
me_SR04_StatePrinter_128x64::StatePrinter _StatePrinter(&Display);

void setup()
{
  Display.begin();
  Display.setContrast(10);

  // Serial.begin(57600);
}

void loop()
{
  float FrameMinDistance = 1e6;
  float FrameMaxDistance = -1.0;

  uint8_t NumNotConnectedErrors = 0;
  uint8_t NumSuccessfullMeasurements = 0;

  for (uint8_t i = 0; i < MeasurementsPerCycle; delay(MeasurementsDelayMs), ++i)
  {
    _SensorDistance.Measure();

    if (_SensorDistance.NotConnected())
      ++NumNotConnectedErrors;

    if (_SensorDistance.HasDistance())
      ++NumSuccessfullMeasurements;

    if (_SensorDistance.HasDistance())
    {
      float sensorDistance = _SensorDistance.DistanceCm();

      FrameMinDistance = min(FrameMinDistance, sensorDistance);
      FrameMaxDistance = max(FrameMaxDistance, sensorDistance);
    }
  }

  bool IsConnected = (NumNotConnectedErrors < MeasurementsPerCycle);
  bool HasDistance = (NumSuccessfullMeasurements > 0);

  Display.clearBuffer();
  _StatePrinter.Display(IsConnected, HasDistance, FrameMinDistance);
  Display.sendBuffer();
}
