/*
  Map readings of A0 pin to white color intensity.

  If pin is unconnected or connected to hanging wire, it's
  effectively antenna and signal is from ether.
*/

/*
  Status: done
  Version: 1.0
  Last mod.: 2020-08-21
*/

#include <FastLED.h>

#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define LED_PIN 10

const int16_t
  NUM_LEDS = 60,
  BRIGHTNESS = 16,
  MAX_CURRENT_MA = 36;

CRGB Leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(Leds, NUM_LEDS);
  // FastLED.setCorrection(TypicalSMD5050);
  // FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_CURRENT_MA);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(0);
  Serial.begin(115200);
}

void loop() {
  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    uint16_t value_10bits = analogRead(A0);
    uint8_t value = value_10bits >> 2;
    // Serial.println(value_10bits);
    Leds[i] = CRGB(value, value, value);
  }
  EVERY_N_MILLISECONDS(240) {
    FastLED.show();
  }
}
