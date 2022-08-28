// Ultrasound distance meter HC-SR04 test.

/*
  Status: stable
  Version: 1.5
  Last mod.: 2022-08-28
*/

#include <me_SR04.h>
#include <me_SR04_StateGetter.h>
#include <me_SR04_StatePrinter_128x32.h>
#include <me_CapacitiveFilter.h>
#include <me_Statistics_TimePoint.h>
#include <me_Statistics_Min.h>
#include <me_Statistics_Max.h>
#include <me_Statistics.Bucket.h>

#include <U8g2lib.h>

const uint8_t
  TriggerPin = 4,
  EchoPin = 4;

const uint32_t
  MeasurementDelayMs = 70;

const float
  DataFilterCapacitance = 4.0,
  CapacitancePerDistance = 0.6;

const uint32_t
  DataFilterAutoFlushSteps = 900,
  AverageFilterNumNodes = 0xFFFFFFFF;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R2);

me_SR04::SR04 Sonar(TriggerPin, EchoPin);
CapacitiveFilter dataSmoother(DataFilterCapacitance, DataFilterAutoFlushSteps);
me_SR04_StatePrinter_128x32::StatePrinter StatePrinter(&Display);
me_Statistics_Min::MinValue minValue(me_Statistics_TimePoint::TimePoint(1000.0, 0));
me_Statistics_Max::MaxValue maxValue(me_Statistics_TimePoint::TimePoint(0.0, 0));
me_Statistics_Bucket::Bucket Bucket;

void setup()
{
  Serial.begin(57600);
  delay(3000);

  Display.begin();
  Display.setContrast(10);
}

float EstimateJitter(float Distance)
{
  float result;

  const float
    MinJitter = 1.2,
    JitterPerDistance = 0.014;

  result = max(MinJitter, Distance * JitterPerDistance);

  return result;
}

void loop()
{
  // Typical jitter is from 2.05% to 3.07% in my tests. Closer the
  // distance, more % of jitter but less jitter delta in cm.

  Sonar.Request();
  me_SR04_StateGetter::State RequestStatus = me_SR04_StateGetter::GetState(&Sonar);

  if (RequestStatus.HasDistance)
  {
    using namespace me_Statistics_TimePoint;
    using namespace me_Statistics_Min;
    using namespace me_Statistics_Max;

    static int32_t TimePosition = 0;

    float sensorDistance = RequestStatus.DistanceCm;

    dataSmoother.SetCapacitance(sensorDistance * CapacitancePerDistance);
    dataSmoother.Add(sensorDistance);

    float smoothedDistance = dataSmoother.Get();

    minValue.Add(TimePoint(smoothedDistance, TimePosition));

    maxValue.Add(TimePoint(smoothedDistance, TimePosition));

    float kt = 0.25;
    if (!minValue.ValueHasChanged())
    {
      float k = pow(smoothedDistance / minValue.Get().Value, kt);
      minValue = MinValue(TimePoint(minValue.Get().Value * k, TimePosition));
    }

    if (!maxValue.ValueHasChanged())
    {
      float k = pow(smoothedDistance / maxValue.Get().Value, kt);
      maxValue = MaxValue(TimePoint(maxValue.Get().Value * k, TimePosition));
    }

    if (!Bucket.Add(sensorDistance))
    {
      float Jitter = EstimateJitter(smoothedDistance);
      Serial.print("Jitter = ");
      Serial.println(Jitter);

      Bucket.Reset(Jitter);
      Bucket.Add(sensorDistance);
      dataSmoother.Flush();
      Serial.println("Bucket recreated.");
    }

    ++TimePosition;

    RequestStatus.DistanceCm = dataSmoother.Get();

    Serial.print(sensorDistance);

    Serial.print(" ");
    Serial.print(dataSmoother.Get());

    Serial.print(" ");
    Serial.print(dataSmoother.GetAccumulatedError());

    Serial.print(" ");
    Serial.print(Bucket.GetCurrentSpan());

    // Serial.print(" ");
    // Serial.print(minValue.Get().Value);

    // Serial.print(" ");
    // Serial.print(maxValue.Get().Value);

    Serial.println();

  }

  Display.clearBuffer();
  StatePrinter.Print(RequestStatus);
  Display.sendBuffer();

  delay(MeasurementDelayMs);
}
