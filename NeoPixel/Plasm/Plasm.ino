#include <FastLED.h>

#define LED_TYPE WS2812
#define LED_PIN 10

const int16_t
  NUM_LEDS = 60,
  LEDS_OFFSET = 35,
  BRIGHTNESS = 64;

const uint8_t
  UPDATES_PER_SECOND = 180 / 60;

const float
  SCALE = 20.0 / (NUM_LEDS - LEDS_OFFSET - 1);

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  delay(300);
  FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS);
  FastLED.setCorrection(TypicalSMD5050);
  // FastLED.setCorrection(UncorrectedColor);
  FastLED.setBrightness(BRIGHTNESS);
  randomSeed(analogRead(A0));
}

void Hilight(uint8_t offset, CRGB color) {
  if (leds[offset] == color) {
    // Serial.print("color already set at offset ");
    // Serial.println(offset);
    return;
  } else {
    // Serial.println(offset);
  }

  leds[offset] = color;
}

void FractalFill(
  uint8_t start,
  uint8_t finish,
  CRGB start_color,
  CRGB finish_color
) {
  if (start > finish)
    return;

  Hilight(start, start_color);

  if (start == finish)
    return;

  Hilight(finish, finish_color);

  if (start + 1 == finish)
    return;

  uint8_t middle = (start + finish) / 2;

  CRGB middle_color = (start_color + finish_color) / 2;
  float noise_value = (float)(finish - start) * SCALE;
  noise_value = (float)(random(0x100) - 0x80) * noise_value / 0x80;
  middle_color = middle_color * noise_value;

  /*
  Serial.print(start); Serial.print(" ");
  Serial.print(finish); Serial.print(" ");
  Serial.print(noise_value); Serial.print(" ");
  Serial.println();
  */

  if (random(10) >= 5) {
    FractalFill(start, middle, start_color, middle_color);
    FractalFill(middle, finish, middle_color, finish_color);
  } else {
    FractalFill(middle, finish, middle_color, finish_color);
    FractalFill(start, middle, start_color, middle_color);
  }
}

uint32_t last_millis = millis();

void loop() {
  uint32_t cur_time = millis();
  uint32_t time_passed = cur_time - last_millis;
  last_millis = cur_time;
  Serial.println(time_passed);

  FractalFill(LEDS_OFFSET, NUM_LEDS - 1, CRGB::Chartreuse, CRGB::CornflowerBlue);
  // FractalFill(LEDS_OFFSET, NUM_LEDS - 1, CRGB::CornflowerBlue, CRGB::Chartreuse);
  //CRGB::Chocolate

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}
