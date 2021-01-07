void setup() {
  Serial.begin(9600);
  uint32_t v1 = 0xFFFFFFFF, v2 = 0;
  Serial.println(v2 - v1);
  Serial.println((float)(v2 - v1));
  Serial.println((float)v2 - v1);
  Serial.println((float)(v2) - v1);
}

void loop() {
}
