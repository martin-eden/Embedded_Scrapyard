const int pause = 1500;
const int duration = 4000;
const int piezo_pin = 9;
const long base = 1;
const int step = 1;

void setup() {
  Serial.begin(9600);
  //38
  //680
  //1980
  //4480
  //5030
  //12930
  //14330--
  //18830
  //25380
  //28680
/*
  tone(piezo_pin, 440, duration);
  delay(pause);
*/

  int i = 0;
  long fq = base;
  for (; !Serial.available(); i++, fq = base + (long)step * i)
  {
    tone(piezo_pin, fq, duration);
    delay(pause);
    Serial.println(fq);
  }
/*
  analogWrite(piezo_pin, 20);
  delay(duration);
  analogWrite(piezo_pin, 0);
  delay(pause);

  analogWrite(piezo_pin, 40);
  delay(duration);
  analogWrite(piezo_pin, 0);
  delay(pause);
*/
}

void loop() {

}




