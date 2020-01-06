/*
  Based on code by Rui Santos, https://randomnerdtutorials.com
  "Complete Guide for Ultrasonic Sensor HC-SR04".
*/

/*
  Status: done
  Version: 1.0
  Last mod.: 2020-01-06
*/

const uint8_t
  TRIGGER_PIN = 11,
  ECHO_PIN = 12;

const uint8_t
  BURST_DURATION_MCR = 10;

const uint16_t
  INTERPULSE_DELAY_MS = 330;

const float
  SPEED_OF_SOUND_MPS = 340, // meters per second
  CM_PER_MCR = SPEED_OF_SOUND_MPS * 100 / 1000000;


void setup()
{
  Serial.begin(9600);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.print("speed of sound: centimeters per microsecond: ");
  Serial.println(CM_PER_MCR, 6);
}

void loop()
{
  uint16_t duration_mcr, cm;

  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(BURST_DURATION_MCR);
  digitalWrite(TRIGGER_PIN, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration_mcr = pulseIn(ECHO_PIN, HIGH);

  // Convert the time into a distance
  cm = (duration_mcr / 2) * CM_PER_MCR;

  Serial.print(duration_mcr);
  Serial.print("mcr ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  delay(INTERPULSE_DELAY_MS);
}
