#include <Servo.h>

const uint8_t
  servoPin = A1,
  hall_sensor_pin = 2;

Servo Servo1;

void setup()
{
  Servo1.attach(servoPin);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(hall_sensor_pin), event_magnet_is_near, RISING);
  pinMode(hall_sensor_pin, INPUT);
  Servo1.write(70);
}

volatile bool magnet_is_near = false;

void event_magnet_is_near()
{
  magnet_is_near = true;
}

void loop()
{
  if (magnet_is_near)
  {
    Serial.print("Magnet is near!\n");

    delay(400);

    Servo1.write(110);
    while (digitalRead(hall_sensor_pin) == LOW);

    detachInterrupt(digitalPinToInterrupt(hall_sensor_pin));

    Servo1.write(90);
    delay(3000);

    attachInterrupt(digitalPinToInterrupt(hall_sensor_pin), event_magnet_is_near, RISING);
    magnet_is_near = false;

    Servo1.write(70);
  }
}