void setup() {
  Serial.begin(9600);

  const float temperature = 23.497;
  const int32_t temperature_rounded = (temperature * 10);

  Serial.println(temperature);
  Serial.println(temperature_rounded);
  Serial.println(int32_t(temperature * 10));

}

void loop() {
}
