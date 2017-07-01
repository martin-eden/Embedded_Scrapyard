/*
  Status: spins
  Last mod.: 2017-06-17
*/


const uint8_t ap_pin = 2;
const uint8_t an_pin = 3;
const uint8_t bp_pin = 4;
const uint8_t bn_pin = 5;


const uint32_t delays_per_circle = 200;
const uint32_t minute = 60e6;

const float max_rpm = 1240;

inline void spin(uint32_t phase_duration) {
  int in_micros;
  if (phase_duration < 16383) {
    in_micros = 1;
  }
  else {
    in_micros = 0;
    phase_duration = phase_duration / 1000;
  }

  if (in_micros) {
    for (int i = 0; i < 10; i++) {
      PORTD = B00101000; delayMicroseconds(phase_duration);
      PORTD = B00100100; delayMicroseconds(phase_duration);
      PORTD = B00010100; delayMicroseconds(phase_duration);
      PORTD = B00011000; delayMicroseconds(phase_duration);
    }
  }
  else {
    for (int i = 0; i < 10; i++) {
      PORTD = B00101000; delay(phase_duration);
      PORTD = B00100100; delay(phase_duration);
      PORTD = B00010100; delay(phase_duration);
      PORTD = B00011000; delay(phase_duration);
    }
  }
}

inline uint32_t rpm2delay(float rpm) {
  return minute / (delays_per_circle * rpm);
}

inline float delay2rpm(uint32_t delay) {
  return minute / (delays_per_circle * delay);
}

void setup() {
  pinMode(ap_pin, OUTPUT);
  pinMode(an_pin, OUTPUT);
  pinMode(bp_pin, OUTPUT);
  pinMode(bn_pin, OUTPUT);

  Serial.begin(9600);
}

const byte status_accelerating = 0;
const byte status_decelerating = 1;

const uint32_t max_delay = 900e2;

void loop() {
  float rpm;
  static int accel_count = 0;
  static unsigned int rest = 0;
  uint32_t new_delay;
  uint32_t delay = max_delay;
  uint32_t min_delay = rpm2delay(max_rpm);
  bool results_printed = false;
  uint32_t a;
  uint32_t b;
  float float_delay;
  float_delay = delay;
  byte status = status_accelerating;
  while (1) {
    switch(status) {
      case status_accelerating:
        if (delay <= min_delay) {
          ;//status = status_decelerating;
          delay = min_delay;
        }
        else {
          accel_count++;
          float_delay = float_delay - (2 * float_delay) / (float)(4 * accel_count + 1);
          delay = uint32_t(float_delay);
        }
        break;
      case status_decelerating:
        if (delay >= max_delay) {
          status = status_accelerating;
          delay = max_delay;
          accel_count = 0;
        }
        else {
          accel_count--;

          float_delay = float_delay + (2 * float_delay) / (float)(4 * accel_count + 1);
          delay = uint32_t(float_delay);

          // Serial.print("accel_count: ");
          // Serial.print(accel_count);
          // Serial.print(", delay: ");
          // Serial.print(delay);
          // Serial.print(", rpm: ");
          // Serial.print(rpm);
          // Serial.print("\n");
        }
        break;
    }
    spin(delay);
  }
}

// 2016-12-18
// 2017-01-05
// 2017-04-05



