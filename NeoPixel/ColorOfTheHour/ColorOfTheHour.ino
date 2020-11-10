#include <FastLED.h>

#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define LED_PIN 10

const int16_t
  NUM_LEDS = 60,
  BRIGHTNESS = 32,
  MAX_CURRENT_MA = 36;

CRGB Leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(Leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setCorrection(TypicalSMD5050);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_CURRENT_MA);
  // FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(0);
}

uint8_t getSaturatedVal(uint8_t v) {
  if (v < 0x80)
    return 0xFF;
  else
    return 0;
}

uint32_t calculate_delay() {
  static uint32_t wished_delay = 1000000L * .20;
  static uint32_t high_margin = 2 * wished_delay;
  static uint32_t low_margin = 10;
  static uint32_t middle = (high_margin + low_margin) / 2;
  static bool is_first_time = true;
  static uint32_t last_micros = micros();

  if (low_margin + 1 >= high_margin)
    digitalWrite(LED_BUILTIN, HIGH);

  uint32_t cur_micros = micros();
  uint32_t time_passed = cur_micros - last_micros;
  last_micros = cur_micros;

  //*
  Serial.println(time_passed);

  Serial.print(low_margin);
  Serial.print(" ");
  Serial.print(middle);
  Serial.print(" ");
  Serial.print(high_margin);
  Serial.println();
  //*/

  if (is_first_time)
    // can't do decision yet
    is_first_time = false;
  else {
    if (time_passed > wished_delay)
      high_margin = middle;
    else
      low_margin = middle;
    middle = (high_margin + low_margin) / 2;
  }

  return middle / NUM_LEDS;
}

void loop() {
  static uint8_t rainbowStartHue = HUE_GREEN;
  static uint32_t delay_mcr;

  delay_mcr = calculate_delay();

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

    // Leds[i] = CRGB::Black;

    FastLED.show();

    if (delay_mcr < 0x4000)
      delayMicroseconds(delay_mcr);
    else
      delay(delay_mcr / 1000);

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
}
