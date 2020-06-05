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

static inline int8_t sign(float val) {
  if (val < 0)
    return -1;
  if (val==0)
    return 0;
  return 1;
}

float clamp_ui8(float val) {
  if (val < 0)
    return val + 0xFF;
  if (val > 0xFF)
    return val - 0xFF;
  return val;
}

float saturate_ui8(float val) {
  return constrain(val, 0, 0xFF);
}

float pow_table[NUM_LEDS];

void FillPowTable() {
  const float scale = (float) 8 / NUM_LEDS;
  const float arg_offset = -4;
  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    float x = i * scale + arg_offset;
    x = abs(x);
    float pow_val = pow(abs(x), 1.327);
    pow_table[i] = pow_val;
  }
}

void FillWave(float offs_rad) {
  const float scale = (float) 8 / NUM_LEDS;
  const float arg_offset = -4;
  START_HUE = clamp_ui8(START_HUE + 1.15);
  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    float x = i * scale + arg_offset;
    x = abs(x);
    float pow_val = pow_table[i];
    float func_val = 20 * pow_val * sign(x) * sin(offs_rad + pow_val);
    // CHSV pixel = CHSV(START_HUE + func_val, 192, 0x7F);
    CHSV pixel = CHSV(START_HUE, 0xFF, saturate_ui8(0x7F + func_val));
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
  // FastLED.setMaxPowerInVoltsAndMilliamps(5, 100);
  FastLED.setCorrection(TypicalSMD5050);
  // FastLED.setCorrection(UncorrectedColor);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(1);
  FastLED.clear();
  FillPowTable();
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
  offs_angle = offs_angle + 0.2;

  FastLED.show();
  // FastLED.delay(60000 / UPDATES_PER_MINUTE);
}
