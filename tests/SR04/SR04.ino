// Ultrasound distance meter HC-SR04 test.

/*
  Status: stable
  Version: 1.6
  Last mod.: 2022-09-07
*/

#include <me_SR04.h>
#include <me_SR04_StateGetter.h>
#include <me_SR04_StatePrinter_128x32.h>
#include <me_CapacitiveFilter.h>
#include <me_Statistics_TimePoint.h>
#include <me_Statistics.Bucket.h>
#include <me_Stat_AverageFilter.h>

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

me_Statistics_TimePoint::TimePoint DefaultMinValue(1000.0, 0);
me_Statistics_TimePoint::TimePoint DefaultMaxValue(0.0, 0);

me_Statistics_Bucket::Bucket Bucket(0.0);
me_Stat_AverageFilter::Filter Averager(10);

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
    MinJitter = 1.1,
    JitterPerDistance = 0.014;

  result = max(MinJitter, Distance * JitterPerDistance);

  return result;
}

float FloatMap(float Value, float SrcRangeMin, float SrcRangeMax, float DestRangeMin, float DestRangeMax)
{
  if (
    (SrcRangeMin >= SrcRangeMax) ||
    (DestRangeMin >= DestRangeMax)
  )
    return -1.0;

  Value = max(Value, SrcRangeMin);
  Value = min(Value, SrcRangeMax);

  float SrcOffset = Value - SrcRangeMin;
  float SrcSpan = SrcRangeMax - SrcRangeMin;
  float SrcPos = SrcOffset / SrcSpan;
  float DestPos = SrcPos;
  float DestSpan = DestRangeMax - DestRangeMin;
  float DestOffset = DestPos * DestSpan;
  float Result = DestOffset + DestRangeMin;

  return Result;
}

void DisplayFineMark(
  U8G2* Display,
  me_Statistics_Bucket::Bucket* Bucket,
  float DistanceCm
)
{
  const u8g2_uint_t
    WidgetX = 2,
    WidgetY = 2,
    WidgetWidth = 124,
    WidgetHeight = 28,
    WidgetToothHeight = 4;

  u8g2_uint_t X =
    FloatMap(
      DistanceCm,
      Bucket->GetMinValue(),
      Bucket->GetMaxValue(),
      WidgetX,
      WidgetX + WidgetWidth
    );

  // Display->drawVLine(X, WidgetY, WidgetHeight);

  Display->drawVLine(X, WidgetY, WidgetToothHeight);
  Display->drawVLine(X, WidgetY + WidgetHeight - WidgetToothHeight, WidgetToothHeight);
}

void loop()
{
  // Typical jitter is from 2.05% to 3.07% in my tests. Closer the
  // distance, more % of jitter but less jitter delta in cm.

  Sonar.Ping();
  me_SR04_StateGetter::State RequestStatus = me_SR04_StateGetter::GetState(&Sonar);

  if (RequestStatus.HasDistance)
  {
    using namespace me_Statistics_TimePoint;

    static int32_t TimePosition = 0;

    float sensorDistance = RequestStatus.DistanceCm;

    dataSmoother.SetCapacitance(sensorDistance * CapacitancePerDistance);
    dataSmoother.Add(sensorDistance);

    Averager.Add(sensorDistance);

    if (!Bucket.Add(sensorDistance))
    {
      float Jitter = EstimateJitter(sensorDistance);
      Serial.print("Jitter = ");
      Serial.println(Jitter);

      Bucket.Reset(Jitter);
      Bucket.Add(sensorDistance);

      dataSmoother.Flush();
      dataSmoother.Add(sensorDistance);

      Averager.Reset();
      Averager.Add(sensorDistance);

      Serial.println("Bucket recreated.");
    }

    ++TimePosition;

    RequestStatus.DistanceCm = Averager.Get();

    Serial.print(sensorDistance);

    Serial.print(" ");
    Serial.print(dataSmoother.Get());

    Serial.print(" ");
    Serial.print(Averager.Get());

    Serial.print(" ");
    Serial.print(Bucket.GetMaxSpan());

    Serial.print(" ");
    Serial.print(Bucket.GetMinValue());

    Serial.print(" ");
    Serial.print(Bucket.GetMaxValue());

    Serial.println();
  }

  Display.clearBuffer();
  DisplayFineMark(&Display, &Bucket, RequestStatus.DistanceCm);
  StatePrinter.Print(RequestStatus);
  Display.sendBuffer();

  delay(MeasurementDelayMs);
}
