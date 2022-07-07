// Ultrasound distance meter HC-SR04 test.

/*
  Status: stable
  Version: 1.4
  Last mod.: 2022-07-07
*/

#include <me_SR04.h>
#include <me_SR04_StateGetter.h>
#include <me_SR04_StatePrinter_128x32.h>
#include <me_CapacitiveFilter.h>
#include <me_Statistics_TimePoint.h>
#include <me_Statistics_Min.h>
#include <me_Statistics_Max.h>

#include <U8g2lib.h>

const uint8_t
  TriggerPin = 4,
  EchoPin = 4;

const uint32_t
  MeasurementDelayMs = 100;

const float
  DataFilterCapacitance = 4.0;

const uint32_t
  AverageFilterNumNodes = 0xFFFFFFFF;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R2);

me_SR04::SR04 Sonar(TriggerPin, EchoPin);
CapacitiveFilter dataSmoother(DataFilterCapacitance);
me_SR04_StatePrinter_128x32::StatePrinter StatePrinter(&Display);
me_Statistics_Min::MinValue minValue(me_Statistics_TimePoint::TimePoint(1000.0, 0));
me_Statistics_Max::MaxValue maxValue(me_Statistics_TimePoint::TimePoint(0.0, 0));

void setup()
{
  Serial.begin(57600);
  delay(3000);

  Display.begin();
  Display.setContrast(10);
}

void loop()
{
  Sonar.Request();
  me_SR04_StateGetter::State RequestStatus = me_SR04_StateGetter::GetState(&Sonar);

  if (RequestStatus.HasDistance)
  {
    using namespace me_Statistics_TimePoint;
    using namespace me_Statistics_Min;
    using namespace me_Statistics_Max;

    static int32_t TimePosition = 0;

    float sensorDistance = RequestStatus.DistanceCm;

    dataSmoother.Add(sensorDistance);

    float smoothedDistance = dataSmoother.Get();

    minValue.Add(TimePoint(smoothedDistance, TimePosition));

    maxValue.Add(TimePoint(smoothedDistance, TimePosition));

    if (!minValue.ValueHasChanged())
    {
      float kt = 0.3;
      float k = pow(smoothedDistance / minValue.Get().Value, kt);
      minValue = MinValue(TimePoint(minValue.Get().Value * k, TimePosition));
    }

    if (!maxValue.ValueHasChanged())
    {
      float kt = 0.3;
      float k = pow(smoothedDistance / maxValue.Get().Value, kt);
      maxValue = MaxValue(TimePoint(maxValue.Get().Value * k, TimePosition));
    }

    ++TimePosition;

    RequestStatus.DistanceCm = dataSmoother.Get();

    Serial.print(sensorDistance);

    Serial.print(" ");
    Serial.print(dataSmoother.Get());

    Serial.print(" ");
    Serial.print(minValue.Get().Value);

    Serial.print(" ");
    Serial.print(maxValue.Get().Value);

    Serial.println();
  }

  Display.clearBuffer();
  StatePrinter.Print(RequestStatus);
  Display.sendBuffer();

  delay(MeasurementDelayMs);
}
