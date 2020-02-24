void setup() {
  Serial.begin(9600);
}

void func_1() {
  const uint32_t NUM_RUNS = 1000;
  for (uint32_t i = 0; i < NUM_RUNS; ++i) {
    // uint16_t val = analogRead(A0);
    uint8_t val_2 = digitalRead(12);
  }
}

void func_2() {
  const uint16_t NUM_VALS = 1000;
  uint8_t vals[NUM_VALS];
  for (uint16_t i = 0; i < NUM_VALS; ++i) {
    vals[i] = analogRead(A0);
    // vals[i] = digitalRead(A0);
  }
}

uint32_t measure() {
  uint32_t start_time = micros();
  func_2();
  uint32_t finish_time = micros();
  uint32_t delta = finish_time - start_time;
  return delta;
}

void represent(uint32_t delta) {
  Serial.println(delta);
}

void loop() {
  represent(measure());
  delay(1000);
}

/*
                  µs     kHz
  analogRead  112.00    8.92
  digitalRead   2.45  408.xx
*/
