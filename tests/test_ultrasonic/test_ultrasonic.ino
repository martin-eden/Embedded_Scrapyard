/*
  Based on code by Rui Santos, https://randomnerdtutorials.com
  "Complete Guide for Ultrasonic Sensor HC-SR04".
*/

/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-06-19
*/

const uint8_t
  TRIGGER_PIN = 6,
  ECHO_PIN = 7;

const uint16_t
  PULSE_DURATION_MCR = 100,
  MEASUREMENT_DELAY_MS = 1000;

const float
  SPEED_OF_SOUND_MPS = 340.0,
  MM_PER_MCR = SPEED_OF_SOUND_MPS * 1E3 * 1E-6;

void setup()
{
  Serial.begin(57600);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop()
{
  uint32_t duration_mcr, mm;

  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(PULSE_DURATION_MCR);
  digitalWrite(TRIGGER_PIN, LOW);

  duration_mcr = pulseInLong(ECHO_PIN, HIGH, 30000);

  if (duration_mcr > 0)
  {
    mm = (duration_mcr / 2) * MM_PER_MCR;

    // Serial.print(duration_mcr);
    // Serial.print("mcr ");
    Serial.print(mm);
    // Serial.print("cm");
    Serial.println();
  }

  delay(MEASUREMENT_DELAY_MS);
}
