// (e)lementary (c)ellular (a)utomata - 1d celluar automata

/*
  Status: exploring
  Version: 0.8
  Last mod.: 2020-06-16
*/

#include <FastLED.h>
#include <Keypad.h>

#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define LED_PIN 10

const uint8_t
  NUM_LEDS = 60,
  BRIGHTNESS = 20;

uint8_t Rule = 89; // 195 110 75 137 54 57 165 184 182 165 90 126 30 226 149 89 41

const uint8_t
  KP_NUM_ROWS = 4,
  KP_NUM_COLS = 4;

const char keymap[KP_NUM_ROWS][KP_NUM_COLS] =
  {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
  };

const uint8_t
  rowPins[KP_NUM_ROWS] = {9, 8, 7, 6},
  colPins[KP_NUM_COLS]= {5, 4, 3, 2};

Keypad myKeypad =
  Keypad(makeKeymap(keymap), rowPins, colPins, KP_NUM_ROWS, KP_NUM_COLS);

typedef uint8_t tCell;

CRGB Leds[NUM_LEDS];

const uint16_t FieldSize = NUM_LEDS;
typedef tCell tField[FieldSize];

tField CurField, NewField, CellsHeat;

tCell GetLeftCell(uint8_t pos) {
  if (pos == 0)
    return CurField[pos];
  else
    return CurField[pos - 1];
}

tCell GetRightCell(uint8_t pos) {
  if (pos == FieldSize - 1)
    return CurField[pos];
  else
    return CurField[pos + 1];
}

uint8_t GetNeighborhood(uint8_t pos) {
  uint8_t left, center, right;
  left = GetLeftCell(pos);
  center = CurField[pos];
  right = GetRightCell(pos);

  uint8_t result = (left << 2) + (center << 1) + (right << 0);
  return result;
}

tCell GetBit(uint8_t nbh, uint8_t rule) {
  return (rule >> nbh) & 1;
}

tCell CalcNewCell(uint8_t pos, uint8_t rule) {
  return GetBit(GetNeighborhood(pos), rule);
}

const uint32_t MutateChance = 20000;  // 1 / self

void MutateNewField() {
  for (uint8_t i = 0; i < FieldSize; ++i)
    if (!random(MutateChance))
      NewField[i] = 1 - NewField[i];
}

void FillNewField(uint8_t rule) {
  for (uint8_t i = 0; i < FieldSize; ++i)
    NewField[i] = CalcNewCell(i, rule);
  MutateNewField();
}

const int16_t HeatGranularity = 256 / 3;

uint8_t GetNewHeat(int16_t v) {
  int16_t result = constrain(v, 0, 0xFF);
  result = uint8_t(result);
  return result;
}

uint8_t IncreaseHeat(uint8_t v) {
  int16_t NewHeat = (int16_t)v + (int16_t)HeatGranularity;
  NewHeat = GetNewHeat(NewHeat);
  return NewHeat;
}

uint8_t DecreaseHeat(uint8_t v) {
  int16_t NewHeat = (int16_t)v - (int16_t)HeatGranularity;
  NewHeat = GetNewHeat(NewHeat);
  return NewHeat;
}

void UpdateCellHeat(tCell value, uint8_t pos) {
  if (value)
    CellsHeat[pos] = IncreaseHeat(CellsHeat[pos]);
  else
    CellsHeat[pos] = DecreaseHeat(CellsHeat[pos]);
}

void InitNewField() {
  for (uint8_t i = 0; i < FieldSize; ++i)
    NewField[i] = 0;
  /*
  NewField[FieldSize / 2] = 1;
  return;
  //*/
  const uint8_t NumSeeds = FieldSize / 2;
  for (uint8_t i = 0; i < NumSeeds; ++i) {
    uint8_t p;
    do {
      p = random(FieldSize);
    } while (NewField[p] == 1);
    NewField[p] = 1;
  }
}

void OverwriteCurField() {
  for (uint8_t i = 0; i < FieldSize; ++i) {
    CurField[i] = NewField[i];
    UpdateCellHeat(CurField[i], i);
  }
}

void DrawCell(uint8_t pos) {
  uint8_t led_pos = NUM_LEDS - 1 - pos;
  uint8_t value = CellsHeat[pos];
  value = map(value, 0, 0xFF, 170, 0);
  // Leds[led_pos] = CHSV(0xFF, 0, value);
  Leds[led_pos] = CHSV(value, 0xFF, 0xFF);
}

char GetCellChar(tCell cell) {
  return (cell ? '$' : ' ');
}

void SerialRepresentField() {
  uint8_t NumOnes = 0;
  String line = "";
  line += Rule;
  line += ": ";
  line += GetCellChar(GetLeftCell(0));
  for (uint8_t i = 0; i < FieldSize; ++i) {
    line += GetCellChar(CurField[i]);
    NumOnes += CurField[i];
  }
  line += GetCellChar(GetRightCell(FieldSize - 1));
  line = line + " (" + ((uint16_t)NumOnes * 100 / FieldSize) + "%)";
  Serial.println(line);
}

void DrawCurField() {
  for (uint8_t i = 0; i < FieldSize; ++i)
    DrawCell(i);
  FastLED.show();
  SerialRepresentField();
}

void setup() {
  randomSeed(analogRead(A0));

  Serial.begin(9600);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(Leds, NUM_LEDS);
  // FastLED.setMaxPowerInVoltsAndMilliamps(5, 100);
  FastLED.setCorrection(TypicalSMD5050);
  // FastLED.setCorrection(UncorrectedColor);
  // FastLED.setCorrection(SodiumVapor);

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(1);
  FastLED.clear();

  delay(100);

  Serial.print("Rule ");
  Serial.println(Rule);

  InitNewField();
  OverwriteCurField();
  DrawCurField();
}


void loop() {
  static uint32_t last_millis = millis();
  uint32_t cur_time = millis();
  uint32_t time_passed = cur_time - last_millis;
  last_millis = cur_time;
  // Serial.println(time_passed);

  FillNewField(Rule);
  OverwriteCurField();
  DrawCurField();
  FastLED.delay(160);

  if (
    (myKeypad.getKey() != NO_KEY) ||
    Serial.available()
  ) {
    if (Serial.available()) {
      do
        Serial.read();
      while (Serial.available());
    }
    Rule = random(0xFF);
    Serial.print("Rule ");
    Serial.println(Rule);
    InitNewField();
    OverwriteCurField();

    // setup();
  }
}
