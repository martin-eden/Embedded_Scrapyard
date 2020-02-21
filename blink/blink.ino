const uint8_t led_pin = LED_BUILTIN;

void setup()
{
  pinMode(led_pin, OUTPUT);
}

void loop()
{
  digitalWrite(led_pin, HIGH);
  delay(1600);
  digitalWrite(led_pin, LOW);
  delay(400);
}
