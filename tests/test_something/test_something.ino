const int16_t HeatGranularity = 800;

uint8_t GetNewHeat(const int16_t v) {
  int16_t result = constrain(v, 0, 0xFF);
  result = map(result, 0, 0xFF, 160, 0);
  result = uint8_t(result);
  return result;
}

uint8_t IncreaseHeat(uint8_t v) {
  int16_t NewHeat = (int16_t)v + (int16_t)HeatGranularity;
  NewHeat = GetNewHeat(NewHeat);
  return NewHeat;
}

uint8_t DecreaseHeat(uint8_t v) {
  Serial.print("v: ");
  Serial.println(v);

  int16_t NewHeat = (int16_t)v - (int16_t)HeatGranularity;
  Serial.print("NewHeat: ");
  Serial.println(NewHeat);

  NewHeat = GetNewHeat(NewHeat);
  Serial.print("NewHeat: ");
  Serial.println(NewHeat);

  return NewHeat;
}

void setup() {
  Serial.begin(9600);
  DecreaseHeat(102);
}

void loop() {
}
