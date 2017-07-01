const int analog_pin = 0;

void setup() {
  Serial.begin(9600);
  //Serial.begin(115200);
}

void loop() {
  int val = analogRead(analog_pin);
  // Serial.println(val);
  Serial.write(0xff);
  Serial.write((val >> 8) & 0xff);
  Serial.write(val & 0xff);
}
