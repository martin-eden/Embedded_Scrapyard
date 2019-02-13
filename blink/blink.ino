void setup()
{
  pinMode(13, OUTPUT);
}

void loop()
{
  digitalWrite(13, HIGH);
  delay(1880);
  digitalWrite(13, LOW);
  delay(1880);
}
