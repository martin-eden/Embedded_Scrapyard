const uint8_t
  analog_pin = A0;

void setup()
{
  // Serial.begin(9600);
  Serial.begin(115200);
}

void loop()
{
  uint16_t val = analogRead(analog_pin);
  // Serial.println(val);
  Serial.write(0xff);
  Serial.write((val >> 8) & 0xff);
  Serial.write(val & 0xff);
}
