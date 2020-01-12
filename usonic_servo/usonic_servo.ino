#include <Servo.h>

const uint8_t
  TRIGGER_PIN = 11,
  ECHO_PIN = 12,
  SERVO_PIN = 6;

const uint8_t
  BURST_DURATION_MCR = 10;

const uint16_t
  INTERPULSE_DELAY_MS = 330,
  INIT_SERVO_POS = 35;

const float
  SPEED_OF_SOUND_MPS = 340, // meters per second
  CM_PER_MCR = SPEED_OF_SOUND_MPS * 100 / 1000000;

Servo motor;

void setup()
{
  Serial.begin(9600);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.print("speed of sound: centimeters per microsecond: ");
  Serial.println(CM_PER_MCR, 6);

  motor.attach(SERVO_PIN);
  motor.write(90);
}

uint16_t get_distance_cm()
{
  uint16_t duration_mcr, cm;

  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(BURST_DURATION_MCR);
  digitalWrite(TRIGGER_PIN, LOW);

  duration_mcr = pulseIn(ECHO_PIN, HIGH);

  cm = (duration_mcr / 2) * CM_PER_MCR;

  // Serial.print(duration_mcr);
  // Serial.print("mcr ");
  // Serial.print(cm);
  // Serial.print("cm");
  // Serial.println();

  delay(INTERPULSE_DELAY_MS);

  return cm;
}

void set_servo(uint16_t last_pos, uint16_t final_pos)
{
  // Serial.print("final_pos ");
  // Serial.println(final_pos);
  uint16_t cur_pos;
  int8_t delta;
  if (last_pos <= final_pos)
    delta = 1;
  else
    delta = -1;
  for (cur_pos = last_pos; cur_pos != final_pos; cur_pos += delta)
  {
    motor.write(cur_pos);
    delay(25);
  }
}

const uint16_t
  dist_max = 30,
  dist_min = 2,
  pos_max = 70,
  pos_min = 0;

uint16_t
  last_pos = INIT_SERVO_POS;

void loop()
{
  uint16_t dist = get_distance_cm();
  dist = constrain(dist, dist_min, dist_max);
  Serial.print("dist ");
  Serial.println(dist);
  uint16_t pos = map(dist, dist_min, dist_max, pos_min, pos_max);
  set_servo(last_pos, pos);
  last_pos = pos;
}
