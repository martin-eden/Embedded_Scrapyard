// Testing NeoPixel LED stripe according datasheet for WS2812B (v4)

/*
  Status: not working due hardware
  Last mod.: 2020-08-06
*/

/*
  Protocol uses temporal encoding. Each bit trasferred as

    setHigh()
    sleepNanos(th)
    selLow()
    sleepNanos(tl)

  For zero bit th = 400, tl = 850. For one bit: (800, 450).

  Currently bits are set to one no matter what delay is used.
  I think it's due relatively long execution time of digitalWrite().
*/

const uint8_t
  LedPin = 10,
  MaxNumLeds = 30;

/*
  Delays in nanoseconds. In Arduino environment there is no function
  to delay in nanoseconds. So I've imported related code from
  FastLED library.
*/

#define ClockDurationNs 62.5

const uint16_t
  th1 = 800,
  tl1 = 450,
  th0 = 400,
  tl0 = 850,
  tRes = 60000,

  th1_cycles = th1 / ClockDurationNs,
  tl1_cycles = tl1 / ClockDurationNs,
  th0_cycles = th0 / ClockDurationNs,
  tl0_cycles = tl0 / ClockDurationNs,
  tRes_cycles = tRes / ClockDurationNs;

void setup() {
  pinMode(LedPin, OUTPUT);
  /*
  Serial.begin(9600);
  Serial.println("---");
  Serial.println(th1_cycles);
  Serial.println(tl1_cycles);
  Serial.println(th0_cycles);
  Serial.println(tl0_cycles);
  Serial.println(tRes_cycles);
  */
}

#define FL_NOP __asm__ __volatile__ ("cp r0,r0\n");

template<uint16_t CYCLES> __attribute__((always_inline)) inline void delayCycles() {
  FL_NOP;
  delayCycles<CYCLES-1>();
}

template<> __attribute__((always_inline)) inline void delayCycles<0>() {}

uint8_t NumLeds;

void display() {
  for (uint8_t i = 0; i < NumLeds; ++i) {
    for (uint8_t component = 0; component < 3; ++component) {
      for (uint8_t bit = 8; bit > 0; --bit) {
        PORTB |= (1 << 2);
        // digitalWrite(LedPin, HIGH);
        delayCycles<th0_cycles>;
        PORTB &= !(1 << 2);
        // digitalWrite(LedPin, LOW);
        delayCycles<tl0_cycles>;
      }
    }
  }
  digitalWrite(LedPin, LOW);
  delayMicroseconds(tRes / 1000);
}

void loop() {
  for (NumLeds = 0; NumLeds < MaxNumLeds; ++NumLeds) {
    display();
    delay(200);
  }
  for (NumLeds = MaxNumLeds; NumLeds > 0; --NumLeds) {
    display();
    delay(200);
  }
}
