uint8_t
  heater_pin = 3,
  signal_pin = A0;

uint32_t
  fullheat_time_ms = 60000,
  partheat_time_ms = 90000;

float
  partheat_voltage = 1.4;

uint16_t
  fullheat_value = 1023,
  partheat_value = partheat_voltage / 5 * 1023;

void setup()
{
  Serial.begin(9600);

  pinMode(heater_pin, OUTPUT);
  pinMode(signal_pin, INPUT);
}

void loop()
{
  const uint16_t num_chunks = 100;

  uint32_t chunk_duration;
  uint16_t v;

  chunk_duration = fullheat_time_ms / num_chunks;

  analogWrite(heater_pin, fullheat_value);

  for (uint16_t i = 0; i < num_chunks; i++)
  {
    delay(chunk_duration);
    v = analogRead(signal_pin);
    Serial.println(v);
  }

  chunk_duration = partheat_time_ms / num_chunks;

  const uint16_t num_measurements_per_chunk = 50;
  const uint16_t measurement_duration = chunk_duration / num_measurements_per_chunk;

  analogWrite(heater_pin, partheat_value);

  for (uint16_t i = 0; i < num_chunks; i++)
  {
    uint32_t sum = 0;
    uint16_t num_items = 0;

    for (uint16_t j = 0; j < num_measurements_per_chunk; j ++)
    {
      delay(measurement_duration);
      v = analogRead(signal_pin);
      if (v > 0)
      {
        sum += v;
        num_items++;
      }
    }

    if (num_items)
      v = sum / num_items;
    else
      v = 0;

    Serial.println(v);
  }
}