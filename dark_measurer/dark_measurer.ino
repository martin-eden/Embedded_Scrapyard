const int dark_sensor_pin = A0;
const int REDPin = 11;
const int GREENPin = 10;
const int BLUEPin = 9;

void setup()
{
  pinMode(dark_sensor_pin, INPUT);
  Serial.begin(9600);

  pinMode(REDPin, OUTPUT);
  pinMode(GREENPin, OUTPUT);
  pinMode(BLUEPin, OUTPUT);
  analogWrite(REDPin, 255);
  analogWrite(GREENPin, 255);
  analogWrite(BLUEPin, 255);
}


void loop()
{
  int dark_sensor_value = analogRead(dark_sensor_pin);
  Serial.print("dark_sensor_value: ");
  Serial.print(dark_sensor_value);
  Serial.println();
  delay(250);
}
