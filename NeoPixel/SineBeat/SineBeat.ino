#include <FastLED.h>

#define LED_TYPE WS2812
#define LED_PIN 10

const int16_t
  NUM_LEDS = 60,
  BRIGHTNESS = 64,
  UPDATES_PER_MINUTE = 6000;

float
  START_HUE = 192.0;

CRGB leds[NUM_LEDS];

void FillWave(float offs_rad) {
  const float scale = (float) 8 / NUM_LEDS;
  const float arg_offset = -4;
  START_HUE += 0.05;
  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    float x = i * scale + arg_offset;
    CHSV pixel;
    pixel.hue = START_HUE + 30 * x * sin(offs_rad + pow(abs(x), 1.327));
    pixel.saturation = 0xFF;
    pixel.value = 0xFF;
    leds[i] = pixel;
    /*
    Serial.print(i); Serial.print(" ");
    Serial.print(x); Serial.print(" ");
    Serial.print(pixel.hue); Serial.print("\n");
    //*/
  }
  //Serial.println("---");
}

void setup() {
  randomSeed(analogRead(A0));
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 100);
  FastLED.setCorrection(TypicalSMD5050);
  // FastLED.setCorrection(UncorrectedColor);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(1);
  FastLED.clear();
  delay(100);
}

void loop() {
  static uint32_t last_millis = millis();
  uint32_t cur_time = millis();
  uint32_t time_passed = cur_time - last_millis;
  last_millis = cur_time;
  // Serial.println(time_passed);

  static float offs_angle = 0.0;
  FillWave(offs_angle);
  offs_angle = offs_angle - 0.02;

  FastLED.show();
  // FastLED.delay(60000 / UPDATES_PER_MINUTE);
}
