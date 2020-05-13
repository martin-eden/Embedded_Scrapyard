#include <FastLED.h>

#define LED_TYPE WS2812
#define LED_PIN 10

const int16_t
  NUM_LEDS = 60,
  LEDS_OFFSET = 35,
  LEDS_USED = NUM_LEDS - LEDS_OFFSET - 1,
  BRIGHTNESS = 64;

const uint8_t
  UPDATES_PER_SECOND = 1,  // 180 / 60,
  HUE_DRIFT = 0;

const float
  SCALE = 1.0;

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

void Hilight(
  uint8_t offset,
  CHSV color
) {
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
  CHSV start_color,
  CHSV finish_color
) {
  if (start > finish)
    return;

  Hilight(start, start_color);

  if (start == finish)
    return;

  Hilight(finish, finish_color);

  if (start + 1 == finish)
    return;

  float noise_magnitude = (float) SCALE * (finish - start) / LEDS_USED;
  noise_magnitude = noise_magnitude * 0x100;
  noise_magnitude = constrain(noise_magnitude, 0, 0x100);

  int8_t noise_value = random(-noise_magnitude / 2, noise_magnitude / 2);
  // Serial.println(noise_value);

  fract8 mix_fract = 0x80 - noise_value;

  CHSV middle_color = blend(start_color, finish_color, mix_fract);

  /*
  Serial.print(start); Serial.print(" ");
  Serial.print(finish); Serial.print(" ");
  Serial.print(noise_value); Serial.print(" ");
  Serial.println();
  */

  uint8_t middle = (start + finish) / 2;

  FractalFill(start, middle, start_color, middle_color);
  FractalFill(middle, finish, middle_color, finish_color);
}

void loop() {
  static uint32_t last_millis = millis();
  uint32_t cur_time = millis();
  uint32_t time_passed = cur_time - last_millis;
  last_millis = cur_time;
  // Serial.println(time_passed);

  static CHSV
    start_color = rgb2hsv_approximate(CRGB::Chartreuse),
    finish_color = rgb2hsv_approximate(CRGB::CornflowerBlue);

  // .hue field values may be clamped. So we roll hue independently.
  static uint8_t
    start_color_hue = start_color.hue,
    finish_color_hue = finish_color.hue;

  FractalFill(LEDS_OFFSET, NUM_LEDS - 1, start_color, finish_color);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);

  start_color_hue += HUE_DRIFT;
  finish_color_hue += HUE_DRIFT;

  start_color.hue = start_color_hue;
  finish_color.hue = finish_color_hue;

  /*
  Serial.print(start_color.hue); Serial.print(" ");
  Serial.print(finish_color.hue); Serial.print(" ");
  Serial.println();
  //*/
}
