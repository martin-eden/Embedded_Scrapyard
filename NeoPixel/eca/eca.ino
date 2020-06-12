// (e)lementary (c)ellular (a)utomata - 1d celluar automata

/*
  Status: exploring
  Version: 0.7
  Last mod.: 2020-06-12
*/

#include <FastLED.h>

#define LED_TYPE WS2812
#define LED_PIN 10

const int16_t
  NUM_LEDS = 60,
  BRIGHTNESS = 20;

const uint8_t Rule = 54;

CRGB Leds[NUM_LEDS];

typedef uint8_t tCell;

void DrawCell(tCell value, uint8_t pos) {
  const CRGB
    OffColor = CRGB::Black,
    OnColor = CRGB::White;
  uint8_t led_pos = NUM_LEDS - 1 - pos;
  if (value)
    Leds[led_pos] = OnColor;
  else
    Leds[led_pos] = OffColor;
}

const uint16_t FieldSize = NUM_LEDS;
typedef tCell tField[FieldSize];

tField CurField, NewField;

void DrawCurField() {
  for (uint8_t i = 0; i < FieldSize; ++i) {
    DrawCell(CurField[i], i);
  }
  FastLED.show();
}

void InitCurField() {
  for (uint8_t i = 0; i < FieldSize; ++i)
    CurField[i] = 0;
  /*
  CurField[FieldSize / 2] = 1;
  return;
  //*/
  const uint8_t NumSeeds = 3;
  for (uint8_t i = 0; i < NumSeeds; ++i) {
    uint8_t p;
    do {
      p = random(FieldSize);
    } while (CurField[p] == 1);
    CurField[p] = 1;
  }
}

uint8_t GetNeighborhood(uint8_t pos) {
  uint8_t left, center, right;
  if (pos == 0)
    left = CurField[pos];
  else
    left = CurField[pos - 1];
  center = CurField[pos];
  if (pos == FieldSize - 1)
    right = CurField[pos];
  else
    right = CurField[pos + 1];

  uint8_t result = left + (center << 1) + (right << 2);
  return result;
}

tCell GetBit(uint8_t nbh, uint8_t rule) {
  return (rule >> nbh) & 1;
}

tCell CalcNewCell(uint8_t pos, uint8_t rule) {
  return GetBit(GetNeighborhood(pos), rule);
}

void FillNewField(uint8_t rule) {
  for (uint8_t i = 0; i < FieldSize; ++i)
    NewField[i] = CalcNewCell(i, rule);
}

void setup() {
  randomSeed(analogRead(A0));

  Serial.begin(9600);

  FastLED.addLeds<LED_TYPE, LED_PIN>(Leds, NUM_LEDS);
  // FastLED.setMaxPowerInVoltsAndMilliamps(5, 100);
  FastLED.setCorrection(TypicalSMD5050);
  // FastLED.setCorrection(UncorrectedColor);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(1);
  FastLED.clear();

  delay(100);

  Serial.print("Rule ");
  Serial.println(Rule);
  InitCurField();
  DrawCurField();
}

void loop() {
  static uint32_t last_millis = millis();
  uint32_t cur_time = millis();
  uint32_t time_passed = cur_time - last_millis;
  last_millis = cur_time;
  // Serial.println(time_passed);

  FillNewField(Rule);
  for (uint8_t i = 0; i < FieldSize; ++i) {
    DrawCell(CurField[i], i);
    CurField[i] = NewField[i];
  }
  DrawCurField();
  FastLED.delay(750);
}
