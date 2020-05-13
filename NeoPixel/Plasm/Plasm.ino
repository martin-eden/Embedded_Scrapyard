#include <FastLED.h>

#define LED_TYPE WS2812
#define LED_PIN 10

const int16_t
  NUM_LEDS = 60,
  LEDS_OFFSET = 35,
  LEDS_USED = NUM_LEDS - LEDS_OFFSET - 1,
  BRIGHTNESS = 64;

const uint8_t
  UPDATES_PER_SECOND = 30,  // 180 / 60,
  BACKGROUND_NOISE = 0;

const float
  SCALE = 0.0;

float
  START_HUE_DRIFT = PI / 3,
  FINSH_HUE_DRIFT = -1.0 / 3;

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
    // return;
  } else {
    // Serial.println(offset);
  }

  leds[offset] = color;
}

int8_t GetMixNoise(
  uint8_t noise_magnitude
) {
  int8_t result = random(-noise_magnitude / 2, noise_magnitude / 2);

  return result;
}

int8_t GetHueNoise(
  uint8_t noise_magnitude
) {
  int8_t result = random(-noise_magnitude / 2, noise_magnitude / 2);
  result += random(-BACKGROUND_NOISE / 2, BACKGROUND_NOISE / 2);

  return result;
}

CHSV MixColors(
  CHSV start_color,
  CHSV finish_color,
  uint8_t noise_magnitude
) {
  fract8 mix_fract = 0x80 + GetMixNoise(noise_magnitude);
  CHSV result = blend(start_color, finish_color, mix_fract);
  result.hue += GetHueNoise(noise_magnitude);

  return result;
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

  CHSV middle_color = MixColors(start_color, finish_color, noise_magnitude);

  /*
  Serial.print(middle_color.hue); Serial.print(" ");
  Serial.print(middle_color.saturation); Serial.print(" ");
  Serial.print(middle_color.value); Serial.print(" ");
  Serial.println();
  //*/

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
    start_color = CHSV(128, 255, 255),
    // start_color = rgb2hsv_approximate(CRGB::Chartreuse),
    finish_color = CHSV(0, 255, 255);
    // finish_color = rgb2hsv_approximate(CRGB::CornflowerBlue);

  // .hue field values may be clamped. So we roll hue independently.
  static float
    start_color_hue = start_color.hue,
    finish_color_hue = finish_color.hue;

  FractalFill(LEDS_OFFSET, NUM_LEDS - 1, start_color, finish_color);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);

  start_color_hue += START_HUE_DRIFT;
  finish_color_hue += FINSH_HUE_DRIFT;

  if (abs(start_color_hue - finish_color_hue) >= 0x70) {
    /*
    Serial.print(start_color_hue); Serial.print(" ");
    Serial.print(finish_color_hue); Serial.print(" ");
    Serial.println();
    */

    START_HUE_DRIFT = -START_HUE_DRIFT;
    do {
      start_color_hue += START_HUE_DRIFT;
    } while (abs(start_color_hue - finish_color_hue) >= 0x70);
  }

  start_color.hue = start_color_hue;
  finish_color.hue = finish_color_hue;

  /*
  Serial.print(start_color.hue); Serial.print(" ");
  Serial.print(finish_color.hue); Serial.print(" ");
  Serial.println();
  //*/
}
