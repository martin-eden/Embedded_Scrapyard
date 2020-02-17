const uint32_t
  idle_measurement_delay = uint32_t(1000) * 60 * 12,
  pour_measurement_delay = uint32_t(1000) * 5;

const uint8_t num_blocks = 1;

uint32_t next_request_time[num_blocks] = {0xFFFFFFF0};

uint32_t cur_time;

void setup()
{
  Serial.begin(9600);
  print_status();
}

void print_status()
{
  Serial.print("cur_time: ");
  Serial.print(cur_time, HEX);

  Serial.print(", ");
  Serial.print("next_request_time: {");
  for (uint8_t block_num = 0; block_num < num_blocks; block_num++)
  {
    Serial.print(next_request_time[block_num], HEX);
    Serial.print("; ");
  }
  Serial.print("}");

  Serial.println("");
}

void do_business()
{
  for (uint8_t block_num = 0; block_num < num_blocks; block_num++)
  {
    cur_time = next_request_time[block_num] + 0x10;
    sanity_check();
    next_request_time[block_num] = cur_time + idle_measurement_delay;
  }
}

void sanity_check()
{
  for (uint8_t block_num = 0; block_num < num_blocks; block_num++)
  {
    if ((cur_time < 0x10000000) && (next_request_time[block_num] >= 0x80000000))
      while (1);
  }
}

void loop()
{
  do_business();
  print_status();
}
