// Ultrasound distance meter HC-SR04 test.

/*
  Status: stable
  Version: 1.9
  Last mod.: 2022-11-07
*/

#include <U8g2lib.h>

#include <me_SR04.h>
#include <me_SR04_StateGetter.h>

#include <me_CapacitiveFilter.h>
#include <me_Statistics_TimePoint.h>
#include <me_Statistics.Bucket.h>
#include <me_Stat_AverageFilter.h>

#include <me_SR04_StatePrinter_128x64.h>

const uint8_t
  TriggerPin = 11,
  EchoPin = 12;

const uint32_t
  MeasurementDelayMs = 70;

const float
  DataFilterCapacitance = 4.0,
  CapacitancePerDistance = 0.6;

const uint32_t
  DataFilterAutoFlushSteps = 900,
  AverageFilterNumNodes = 0xFFFFFFFF;

U8G2_SH1106_128X64_NONAME_F_HW_I2C Display(U8G2_R0);

me_SR04::SR04 Sonar(TriggerPin, EchoPin);
CapacitiveFilter Capacitor(DataFilterCapacitance, DataFilterAutoFlushSteps);
me_SR04_StatePrinter_128x64::StatePrinter StatePrinter(&Display);

me_Statistics_TimePoint::TimePoint DefaultMinValue(1000.0, 0);
me_Statistics_TimePoint::TimePoint DefaultMaxValue(0.0, 0);

me_Statistics_Bucket::Bucket Bucket(0.0);
me_Stat_AverageFilter::Filter Averager(10);

void setup()
{
  Display.begin();
  Display.setContrast(10);

  Serial.begin(57600);
}

void loop()
{
  Sonar.Ping();
  me_SR04_StateGetter::State RequestStatus = me_SR04_StateGetter::GetState(&Sonar);

  if (RequestStatus.HasDistance)
  {
    using namespace me_Statistics_TimePoint;

    static int32_t TimePosition = 0;

    float sensorDistance = RequestStatus.DistanceCm;

    Capacitor.SetCapacitance(sensorDistance * CapacitancePerDistance);
    Capacitor.Add(sensorDistance);

    Averager.Add(sensorDistance);

    if (!Bucket.Add(sensorDistance))
    {
      float newJitter = EstimateJitter(sensorDistance);
      // Serial.print("new Jitter: ");
      // Serial.println(newJitter);

      Bucket.SetMaxSpan(newJitter);
      Bucket.Add(sensorDistance);

      Capacitor.Flush();
      Capacitor.Add(sensorDistance);

      Averager.Reset();
      Averager.Add(sensorDistance);

      // Serial.println("Bucket recreated.");
    }

    ++TimePosition;

    RequestStatus.DistanceCm = Capacitor.Get();

    PrintStatus(sensorDistance);
  }

  Display.clearBuffer();
  // DisplayFineMark(&Display, &Bucket, Averager.Get());
  StatePrinter.Display(RequestStatus);
  Display.sendBuffer();

  delay(MeasurementDelayMs);
}

float EstimateJitter(float Distance)
{
  // Typical jitter is from 2.05% to 3.07% in my tests. Closer the
  // distance, more % of jitter but less jitter delta in cm.

  const float
    MinJitter = 2.1,
    JitterPerDistance = 0.03;

  float Result = max(MinJitter, Distance * JitterPerDistance);

  return Result;
}

void PrintStatus(float sensorDistance)
{

  Serial.print("(");
  Serial.print(sensorDistance);
  Serial.print(")");

  Serial.print(" ");
  Serial.print("(");
  Serial.print(Averager.Get());
  Serial.print(")");

  Serial.print(" ");

  Serial.print("(");
  Serial.print(Capacitor.Get());
  Serial.print(")");

  Serial.print(" ");

  Serial.print("(");
  Serial.print(Bucket.GetMinValue());
  Serial.print(" ");
  Serial.print(Bucket.GetMaxValue());
  Serial.print(" ");
  Serial.print("(");
  Serial.print(Bucket.GetCurSpan());
  Serial.print(" ");
  Serial.print(Bucket.GetMaxSpan());
  Serial.print(")");
  Serial.print(")");

  Serial.println();
}

void DisplayFineMark(
  U8G2* Display,
  me_Statistics_Bucket::Bucket* Bucket,
  float DistanceCm
)
{
  const u8g2_uint_t
    X = 2,
    Y = 6,
    Width = 124,
    Height = 12;

  u8g2_uint_t MappedX =
    FloatMap(
      DistanceCm,
      Bucket->GetMinValue(),
      Bucket->GetMaxValue(),
      X,
      X + Width
    );

  Display->drawVLine(MappedX, Y, Height);
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
