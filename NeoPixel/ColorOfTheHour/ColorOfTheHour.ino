#include <FastLED.h>

#define LED_TYPE WS2811
#define NUM_LEDS 60
#define LED_PIN 10
#define COLOR_ORDER GRB

const int16_t
  LEDS_OFFSET = 35,
  BRIGHTNESS = 32;

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType currentBlending;

void setup() {
  Serial.begin(9600);

  delay(3000);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  currentBlending = LINEARBLEND;

  fill_solid(&(leds[0]), LEDS_OFFSET, CRGB(0, 0, 0));
  fill_solid(&(leds[LEDS_OFFSET]), NUM_LEDS - LEDS_OFFSET, CRGB(255, 0, 0));
  FastLED.show();
}

void loop() {
}
