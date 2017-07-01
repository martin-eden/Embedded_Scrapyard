int REDPin = 11;
int GREENPin = 10;
int BLUEPin = 9;

void setup()
{
  pinMode(REDPin, OUTPUT);
  pinMode(GREENPin, OUTPUT);
  pinMode(BLUEPin, OUTPUT);
  analogWrite(REDPin, 255);
  analogWrite(GREENPin, 255);
  analogWrite(BLUEPin, 255);
}

void loop()
{
}
