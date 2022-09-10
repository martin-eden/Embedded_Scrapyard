#include <SoftwareSerial.h>

SoftwareSerial softSerial(8, 9); // RX, TX

void setup()
{
  Serial.begin(57600);
  while (!Serial);
  softSerial.begin(57600);
}

void loop()
{
  if (softSerial.available())
  {
    Serial.write(softSerial.read());
  }
  if (Serial.available())
  {
    softSerial.write(Serial.read());
  }
}
