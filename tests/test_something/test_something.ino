void setup() {
  Serial.begin(9600);
  const uint32_t DESIRED_POUR_CYCLE_H = 48;
  float numHoursPassed = 52.1;
  float correctionCoeff = DESIRED_POUR_CYCLE_H / numHoursPassed;
  uint32_t pour_duration_ms = 120000;
  pour_duration_ms = pour_duration_ms * correctionCoeff;
  Serial.println(DESIRED_POUR_CYCLE_H);
  Serial.println(numHoursPassed);
  Serial.println(correctionCoeff);
  Serial.println(pour_duration_ms);

}

void loop() {
}
