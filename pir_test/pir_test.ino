// Based on "Henry's Bench"
// HC-SR501 Motion Detector

int ledPin = 13;
int pirPin = 2;

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);

  digitalWrite(ledPin, LOW);
}

void loop()
{
  digitalWrite(ledPin, digitalRead(pirPin));
}
