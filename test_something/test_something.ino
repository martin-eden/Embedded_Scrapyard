void setup() {
  Serial.begin(9600);
}

void serialEvent() {
  handle_command();
}

void loop() {
}

int parse_block_num(char c) {
  if (c == '0')
    return 0;
  else if (c == '1')
    return 1;
  else
    ;
  return -1;
}

int parse_on_off(char c) {
  if (c == '0')
    return 0;
  else if (c == '1')
    return 1;
  else
    ;
  return -1;
}

const uint8_t cmd_measure = 'M';
const uint8_t cmd_execute = 'X';
const uint8_t cmd_get_state = 'G';
const uint8_t cmd_set_state = 'S';

void handle_command() {
  char cmd = Serial.peek();
  uint8_t data_length = Serial.available();
  uint8_t block_num;
  uint8_t on_off;
  switch (cmd) {
    case cmd_measure:
      if (data_length < 2)
        break;
      Serial.read();
      block_num = parse_block_num(Serial.read());
      break;
    case cmd_execute:
      if (data_length < 3)
        break;
      Serial.read();
      block_num = parse_block_num(Serial.read());
      on_off = parse_on_off(Serial.read());
      break;
    case cmd_get_state:
      if (data_length < 2)
        break;
      Serial.read();
      Serial.read();
      break;
    case cmd_set_state:
      if (data_length < 2)
        break;
      Serial.read();
      Serial.read();
      break;
    default:
      Serial.read();
      Serial.print(cmd);
      break;
  }
}
