const uint8_t
  outputPin = 13;

void setup()
{
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW);
}

void loop()
{
  if (Serial.available())
  {
    uint32_t durationMs;
    uint32_t delayMs;
    uint16_t numRepeats;
    durationMs = Serial.parseInt();
    delayMs = Serial.parseInt();
    if (delayMs == 0)
    {
      delayMs = durationMs;
    }
    numRepeats = Serial.parseInt();
    if (numRepeats == 0)
    {
      numRepeats = 1;
    }
    Serial.println(durationMs);
    Serial.println(numRepeats);

    for (uint16_t i = 0; i < numRepeats; ++i)
    {
      digitalWrite(outputPin, HIGH);
      delay(durationMs);
      digitalWrite(outputPin, LOW);
      delay(delayMs);
    }
  }
}
