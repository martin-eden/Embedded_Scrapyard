/*
  Status: stable
  Last mod.: 2017-04-05
*/

/*
  Spins hdd 3-phase motor to given RPM and maintain rotation.
  --
  Currently spins up motor powered from 5V up to 3500 rpm.
  Above 3500 rpm occurs
    * some resonance frequency or
    * some frequency artifact in L298N board or
    * real torque becomes too low.
  Anyway motor "bzuus" and slowly stopping but electomagnet switches
  still heared and feeled if gently rotate motor by hand.
  --
  Burned today two same cheap L298N boards from eBay. First disabled
  OUT1, OUT2 ports on both boards. Today broke OUT3, OUT4 ports on
  first board. So I have just two phases from three needed to spin in
  right direction. Tweaked code to use disc inertion to maintain
  some rotation rate.

  Recommendataion: do not put 12V on both (OUT1, OUT2) or (OUT3, OUT4)
*/


const uint8_t phase_1_pin = 2;
const uint8_t phase_2_pin = 3;
const uint8_t phase_3_pin = 4;


const uint32_t blocks_per_circle = 4;
const uint32_t delays_per_block = 6;
const uint32_t delays_per_circle = delays_per_block * blocks_per_circle;
const uint32_t minute = 60e6;

const float max_rpm = 240;

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
    for (int i = 0; i < 1; i++) {
      PORTD = B00000100; delayMicroseconds(phase_duration);
      PORTD = B00001100; delayMicroseconds(phase_duration);
      PORTD = B00001000; delayMicroseconds(phase_duration);
      PORTD = B00011000; delayMicroseconds(phase_duration);
      PORTD = B00010000; delayMicroseconds(phase_duration);
      PORTD = B00010100; delayMicroseconds(phase_duration);
    }
  }
  else {
    for (int i = 0; i < 1; i++) {
      PORTD = B00000100; delay(phase_duration);
      PORTD = B00001100; delay(phase_duration);
      PORTD = B00001000; delay(phase_duration);
      PORTD = B00011000; delay(phase_duration);
      PORTD = B00010000; delay(phase_duration);
      PORTD = B00010100; delay(phase_duration);
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
  pinMode(phase_1_pin, OUTPUT);
  pinMode(phase_2_pin, OUTPUT);
  pinMode(phase_3_pin, OUTPUT);

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


