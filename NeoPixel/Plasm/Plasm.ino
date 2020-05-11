#include <FastLED.h>

#define LED_TYPE WS2812
#define LED_PIN 10

const int16_t
  NUM_LEDS = 60,
  LEDS_OFFSET = 35,
  BRIGHTNESS = 64;

const uint32_t
  UPDATES_PER_SECOND = 60;

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  delay(300);
  FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  randomSeed(analogRead(A0));
}

void Hilight(uint8_t offset, CRGB color) {
  if (leds[offset] == color) {
    Serial.print("color already set at offset ");
    Serial.println(offset);
  } else {
    // Serial.println(offset);
  }

  leds[offset] = color;
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FractalFill(uint8_t start, uint8_t finish, CRGB color) {
  /*
  Serial.print(start);
  Serial.print(" ");
  Serial.print(finish);
  Serial.println();
  */
  if (start > finish)
    return;

  Hilight(start, color);

  if (start == finish)
    return;

  Hilight(finish, color);

  if (start + 1 == finish)
    return;

  uint8_t middle = (start + finish) / 2;
  if (random(10) >= 5) {
    FractalFill(start + 1, middle, color);
    FractalFill(middle + 1, finish - 1, color);
  } else {
    FractalFill(middle + 1, finish - 1, color);
    FractalFill(start + 1, middle, color);
  }
}

uint32_t last_millis = millis();

void loop() {
  uint32_t cur_time = millis();
  uint32_t time_passed = cur_time - last_millis;
  last_millis = cur_time;
  Serial.println(time_passed);

  FractalFill(LEDS_OFFSET, NUM_LEDS - 1, CRGB::Chartreuse);
  FractalFill(LEDS_OFFSET, NUM_LEDS - 1, CRGB::Chocolate);
}
