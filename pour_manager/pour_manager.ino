// Measures soil dryness and pours if needed.

const char
  code_name[] = "Pour manager",
  code_descr[] = "Measures soil dryness and pours if needed.",
  version[] = "2.4.1";

/*
  Status: stable
  Last mod.: 2020-12-23
*/

/*
  Requirements:
    * 4-digit LED display
    * Capacitive soil moisture sensor
    * Relay switch
    * Water pump motor
*/

#include <me_switch.h>
#include <TM1637Display.h>

const uint8_t
  MEASURER_SIGNAL_PIN = A0,

  MOTOR_CONTROL_PIN = 2,

  DISPLAY_INPUT_PIN = 5,
  DISPLAY_CLOCK_PIN = 6;

const uint16_t
  MEASURER_RANGE_LOW = 310,
  MEASURER_RANGE_HIGH = 312,
  MEASURER_ABS_LOW = 240,
  MEASURER_ABS_HIGH = 650;

const bool
  MEASURER_HIGH_MEANS_DRY = true;

const uint32_t
  IDLE_MEASUREMENT_DELAY = uint32_t(1000) * 5, //uint32_t(1000) * 7,
  POUR_MEASUREMENT_DELAY = uint32_t(1000) / 2; //uint32_t(1000) * 7;

c_switch motor = c_switch(MOTOR_CONTROL_PIN);
TM1637Display display(DISPLAY_CLOCK_PIN, DISPLAY_INPUT_PIN);

void setup() {
  Serial.begin(9600);

  display.setBrightness(0x0F);

  // Assure normal business logic was done before printing status:
  loop();
}

const char
  CMD_GET_STATE = 'G';

void print_usage() {
  String msg = "";
  msg =
    msg +
    "----------------------------------------------------\n" +
    "Code: \"" + code_name + "\"\n" +
    "Description: " + code_descr + "\n" +
    "Version: " + version + "\n" +
    "----------------------------------------------------\n" +
    "Wiring:\n"
    "  MOTOR_CONTROL_PIN: " + MOTOR_CONTROL_PIN + "\n" +
    "  MEASURER_SIGNAL_PIN: " + MEASURER_SIGNAL_PIN + "\n" +
    "  DISPLAY_INPUT_PIN: " + DISPLAY_INPUT_PIN + "\n" +
    "  DISPLAY_CLOCK_PIN: " + DISPLAY_CLOCK_PIN + "\n" +
    "\n" +
    "Settings:\n" +
    "  MEASURER_RANGE_LOW: " + MEASURER_RANGE_LOW + "\n" +
    "  MEASURER_RANGE_HIGH: " + MEASURER_RANGE_HIGH + "\n" +
    "  MEASURER_ABS_LOW: " + MEASURER_ABS_LOW + "\n" +
    "  MEASURER_ABS_HIGH: " + MEASURER_ABS_HIGH + "\n" +
    "\n";
  Serial.print(msg);

  Serial.print("  Delays:\n");
  Serial.print("    IDLE_MEASUREMENT_DELAY: ");
  Serial.println((float)IDLE_MEASUREMENT_DELAY / 1000);
  Serial.print("    POUR_MEASUREMENT_DELAY: ");
  Serial.println((float)POUR_MEASUREMENT_DELAY / 1000);

  Serial.print("  High values means dry?: ");
  Serial.println(MEASURER_HIGH_MEANS_DRY);

  msg = "";
  msg =
    msg +
    "Usage:" + "\n" +
    "  " + CMD_GET_STATE + " - Print current status.\n" +
    "\n";
  Serial.print(msg);
}

void handle_command() {
  char cmd = Serial.peek();
  uint8_t data_length = Serial.available();
  int8_t on_off;
  uint8_t value;
  switch (cmd) {
    case CMD_GET_STATE:
      Serial.read();
      print_status();
      break;
    default:
      Serial.read();
      print_usage();
      break;
  }
}

int16_t get_raw_value() {
  return analogRead(MEASURER_SIGNAL_PIN);
}

bool is_line_problem() {
  int16_t result = get_raw_value();
  return (result <= MEASURER_ABS_LOW) || (result >= MEASURER_ABS_HIGH);
}

const int16_t BASE_FORCE_TO_CHANGE = 20;

float
  base_value = 0,
  deviation = 0;
uint32_t
  num_changes = 0;

int16_t get_humidity() {
  float result = get_raw_value();
  int16_t raw_result = result;

  float delta = result - base_value;
  deviation += delta;
  if (abs(deviation) >= BASE_FORCE_TO_CHANGE) {
    if (num_changes == 0)
      base_value = result;
    else
      base_value += deviation / num_changes;
    deviation = 0;
    num_changes = 0;
  }
  result = base_value;
  ++num_changes;

  display.showNumberDec((int16_t)result, false);

  Serial.print("get_humidity: ");
  Serial.print(result, 4);
  Serial.print(" ");
  Serial.print(raw_result);
  Serial.print(" ");
  Serial.print(deviation, 4);
  Serial.println("");

  if (is_line_problem()) {
    result = -1;
    delay(500);
    display.showNumberDec(result, false);
    delay(500);
  }

  return result;
}

void print_status() {
  String msg = "";

  msg = "";
  msg =
    msg +
    "  Motor: " + motor.is_on() + "\n";
  Serial.print(msg);

  Serial.print("\n");
}

void motor_on() {
  if (motor.is_off()) {
    motor.switch_on();
    Serial.println("Motor ON.");
  }
}

void motor_off() {
  if (motor.is_on()) {
    motor.switch_off();
    Serial.println("Motor OFF.");
  }
}

bool ui32_gte(uint32_t a, uint32_t b) {
  return
    (a >= b) ||
    (
      (a < b) &&
      (b - a >= 0x40000000)
    );
}

const uint8_t
  STATE_IDLE = 0,
  STATE_POURING = 1,
  STATE_COOLDOWN = 2;

uint8_t state = STATE_IDLE;
uint32_t next_state_update_time = millis();

void motor_starter(int16_t cur_hum, uint32_t cur_time) {
  if (
    (state == STATE_IDLE) &&
    (
      (MEASURER_HIGH_MEANS_DRY && (cur_hum > MEASURER_RANGE_HIGH)) ||
      (!MEASURER_HIGH_MEANS_DRY && (cur_hum < MEASURER_RANGE_LOW))
    )
  ) {
    motor_on();
  }
}

void motor_stopper(int16_t cur_hum, uint32_t cur_time) {
  if (
    (state == STATE_POURING) &&
    (
      ui32_gte(cur_time, next_state_update_time) ||
      (MEASURER_HIGH_MEANS_DRY && (cur_hum < MEASURER_RANGE_LOW)) ||
      (!MEASURER_HIGH_MEANS_DRY && (cur_hum > MEASURER_RANGE_HIGH)) ||
      (cur_hum == -1)
    )
  ) {
    motor_off();
  }
}

void update_state(uint32_t cur_time) {
  if ((state == STATE_IDLE) && motor.is_on()) {
    state = STATE_POURING;
    next_state_update_time = cur_time + 20000;
  } else if ((state == STATE_POURING) && motor.is_on()) {
    state = STATE_COOLDOWN;
    next_state_update_time = cur_time + 180000;
  } else if ((state == STATE_COOLDOWN) && ui32_gte(cur_time, next_state_update_time)) {
    state = STATE_IDLE;
  }
}

void do_business() {
  int16_t cur_hum = get_humidity();
  uint32_t cur_time = millis();

  motor_starter(cur_hum, cur_time);
  motor_stopper(cur_hum, cur_time);
  update_state(cur_time);
}

void serialEvent() {
  handle_command();
}

bool is_time_to_work(uint32_t cur_time, uint32_t next_request_time) {
  return ui32_gte(cur_time, next_request_time);
}

uint32_t get_next_request_time(uint32_t cur_time, bool motor_is_on) {
  uint32_t result;
  if (motor_is_on)
    result = cur_time + POUR_MEASUREMENT_DELAY;
  else
    result = cur_time + IDLE_MEASUREMENT_DELAY;
  return result;
}

uint32_t next_request_time = 0;

void loop() {
  uint32_t cur_time = millis();

  bool time_to_work = is_time_to_work(cur_time, next_request_time);
  if (!time_to_work)
    return;

  do_business();

  next_request_time = get_next_request_time(cur_time, motor.is_on());
}

/*
  2016-03-16
  [...]
  2020-03-14
    Reduced mostly expreimental features of "Flower frined" to minimum.
  2020-11-14
    Reduced code even more.
*/
