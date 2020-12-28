#include <me_PIDCSmoother.h>
#include <me_CapacitiveFilter.h>

PIDCSmoother pIDCSmoother1 = PIDCSmoother((float) 0.2, (float) 0.00075, (float) 0.0, 50);
CapacitiveFilter capacitiveFilter1 = CapacitiveFilter(21);
CapacitiveFilter capacitiveFilter2 = CapacitiveFilter(10);

void setup() {
  Serial.begin(9600);
}

void loop() {
  float value = analogRead(A0);
  capacitiveFilter1.addValue(value);
  float smoothedValue1 = capacitiveFilter1.getValue();
  capacitiveFilter2.addValue(value);
  float smoothedValue2 = capacitiveFilter2.getValue();
  pIDCSmoother1.addValue(value);
  float smoothedValue3 = pIDCSmoother1.getValue();
  // derivativeSmoother1.addValue(value);
  // float smoothedValue3 = derivativeSmoother1.getValue();

  String msg = "";
  // msg = msg + value + " " + smoothedValue1;
  // msg = msg + smoothedValue1;
  msg = msg + smoothedValue1 + " " + smoothedValue2 + " " + value;
  // msg = msg + value + " " + smoothedValue1 + " " + smoothedValue2 + " " + smoothedValue3;

  Serial.println(msg);

  delay(500);
}