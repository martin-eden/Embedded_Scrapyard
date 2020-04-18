#include <FastLED.h>

#define LED_TYPE WS2811
#define NUM_LEDS 60
#define LED_PIN 6
#define COLOR_ORDER GRB

const int16_t
  LEDS_OFFSET = 43,
  BRIGHTNESS = 32,
  PALETTE_CHANGE_PERIOD_S = 180;

CRGB leds[NUM_LEDS];

#define UPDATES_PER_MINUTE 1800

CRGBPalette16 currentPalette;
TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
  Serial.begin(9600);
  delay(3000); // power-up safety delay

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(BRIGHTNESS);
  // currentBlending = NOBLEND;
  currentBlending = LINEARBLEND;

  randomSeed(analogRead(A0));
  SetupTotallyRandomPalette();
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
  for( int i = 0; i < 16; i = i + 2) {
    currentPalette[i] = CHSV(random8(), 255, 255);
    currentPalette[i + 1] = currentPalette[i];
  }
}

static uint8_t lastSecond = 99;

void ChangePalettePeriodically() {
  uint8_t currentSecond = (millis() / 1000) % PALETTE_CHANGE_PERIOD_S;

  if ((currentSecond == 0) && (currentSecond != lastSecond))
    SetupTotallyRandomPalette();

  lastSecond = currentSecond;
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }

  fill_solid(&(leds[0]), LEDS_OFFSET, 0);
}

int8_t inc = 1;

void loop() {
  ChangePalettePeriodically();

  static uint8_t startIndex = 0;
  if (random(1000) >= 999)
    inc = -inc;

  startIndex += inc;

  FillLEDsFromPaletteColors(startIndex);

  FastLED.show();
  FastLED.delay(60000 / UPDATES_PER_MINUTE);
}
