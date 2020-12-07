// Measures soil dryness and pours if needed.

const char
  code_name[] = "Pour manager",
  code_descr[] = "Measures soil dryness and pours if needed.",
  version[] = "2.2.0";

/*
  Status: stable
  Last mod.: 2020-12-07
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
  MEASURER_RANGE_LOW = 315,
  MEASURER_RANGE_HIGH = 330,
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

uint32_t next_request_time;

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

uint32_t cur_time;

void print_status() {
  String msg = "";

  msg = "";
  msg =
    msg +
    "  Motor: " + motor.is_on() + "\n";
  Serial.print(msg);

  Serial.print("\n");
}

bool is_time_to_work() {
  return
    (cur_time >= next_request_time) ||
    (
      (cur_time < 0x10000000) &&
      (next_request_time >= 0x80000000)
    );
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

void update_next_request_time() {
  if (motor.is_on())
    next_request_time = cur_time + POUR_MEASUREMENT_DELAY;
  else
    next_request_time = cur_time + IDLE_MEASUREMENT_DELAY;
}

void do_business() {
  bool time_to_work = is_time_to_work();

  if (!time_to_work)
    return;

  int cur_hum = get_humidity();
  if (cur_hum > MEASURER_RANGE_HIGH) {
    if (MEASURER_HIGH_MEANS_DRY)
      motor_on();
    else
      motor_off();
  }
  if (cur_hum < MEASURER_RANGE_LOW) {
    if (MEASURER_HIGH_MEANS_DRY)
      motor_off();
    else
      motor_on();
  }

  if (cur_hum == -1)
    motor_off();

  update_next_request_time();
}

void serialEvent() {
  handle_command();
}

void loop() {
  cur_time = millis();
  do_business();
}

/*
  2016-03-16
  [...]
  2020-03-14
    Reduced mostly expreimental features of "Flower frined" to minimum.
  2020-11-14
    Reduced code even more.
*/
