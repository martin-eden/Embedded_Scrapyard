void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Hardware serial pass-through.");
}

void loop() {
  if (Serial.available()) {
    // Serial.print("Serial.available(): ");
    // Serial.println(Serial.available());
    Serial1.write(Serial.read());
  }

  if (Serial1.available()) {
    // Serial.print("Serial_1.available(): ");
    // Serial.println(Serial1.available());
    Serial.write(Serial1.read());
  }
}