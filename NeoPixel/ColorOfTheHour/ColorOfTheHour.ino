#include <FastLED.h>

#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define LED_PIN 10

const int16_t
  NUM_LEDS = 60,
  BRIGHTNESS = 32,
  MAX_CURRENT_MA = 50;

CRGB Leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(Leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_CURRENT_MA);
  FastLED.setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(0);
}

uint8_t getSaturatedVal(uint8_t v) {
  if (v < 0x80)
    return 0xFF;
  else
    return 0;
}

void loop() {
  static uint8_t rainbowStartHue = HUE_GREEN;

  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    EVERY_N_MILLISECONDS(1500) {
      fill_rainbow(&(Leds[0]), NUM_LEDS, rainbowStartHue, ceil(0x100 / NUM_LEDS) / 2);
      rainbowStartHue += 2;
    }

    CRGB origColor = Leds[i];

    /*
    const uint8_t hueAddon = 120;
    CHSV hsvColor = rgb2hsv_approximate(origColor);
    uint16_t newHue = hsvColor.hue + hueAddon;
    if (newHue >= 0x100)
      newHue -= 0x100;
    hsvColor.hue = newHue;
    hsvColor.saturation = 0xFF;
    hsvColor.value = 0xFF;
    Leds[i] = hsvColor;
    */

    Leds[i] = CRGB::Black;

    FastLED.show();
    delayMicroseconds(874000 / NUM_LEDS);

    Leds[i] = origColor;

/*
    Leds[i] = -Leds[i];
    Leds[i] /= 2;
    Leds[i + 1] = -Leds[i + 1];
    Leds[i + 1] /= 2;
    FastLED.delay(300);

    Leds[i] *= 2;
    Leds[i] = -Leds[i];
    Leds[i + 1] *= 2;
    Leds[i + 1] = -Leds[i + 1];
*/
  }
  Serial.println(".");
}
