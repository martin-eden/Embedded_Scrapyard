const uint8_t piezo_pin = A0;

const uint16_t duration = 1000;
const uint16_t pause = 500;

const uint32_t start_fq = 110;
const float k = 1.2;
const uint32_t max_fq = 64000;

void setup()
{
  pinMode(piezo_pin, OUTPUT);

  Serial.begin(9600);

  uint32_t fq = start_fq;
  while (1)
  {
    if (Serial.available() || (fq > max_fq))
      break;

    Serial.println(fq);
    tone(piezo_pin, fq, duration);
    delay(duration + pause);

    fq = k * fq;
  }
}

void loop()
{

}
