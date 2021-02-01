// Measures soil dryness and pours if needed.

const char
  code_name[] = "Pour manager",
  code_descr[] = "Measures soil dryness and pours if needed.",
  version[] = "2.7.2";

/*
  Status: stable
  Last mod.: 2021-02-01
*/

/*
  Requirements:
    * Capacitive soil moisture sensor
    * Relay switch
    * Water pump motor
    * 4-digit LED display
*/

#include <me_switch.h>
#include <me_CapacitiveFilter.h>
#include <me_EepromCircularStack.h>
#include <TM1637Display.h>

const uint8_t
  SENSOR_PIN = A0,

  RELAY_PIN = A2,

  DISPLAY_INPUT_PIN = A4,
  DISPLAY_CLOCK_PIN = A5;

const uint32_t
  MAX_MOTOR_ON_TIME_MS = 100 * 1000L,
  GET_HUMIDITY_TICK_MS = 1 * 1000L,
  IDLE_DURATION_MS = 60 * 1000L,
  PRE_POUR_DURATION_MS = 300 * 1000L,
  BASE_POUR_DURATION_MS = 8 * 1000L,
  POST_POUR_DURATION_MS = 900 * 1000L,
  DESIRED_POUR_CYCLE_H = 48;

const uint16_t
  SENSOR_THRESHOLD = 324,
  SENSOR_ABS_LOW = 240,
  SENSOR_ABS_HIGH = 650;

const bool
  SENSOR_HIGH_MEANS_DRY = true;

const float
  DATA_FILTER_CAPACITANCE = 11;

c_switch motor = c_switch(RELAY_PIN);
CapacitiveFilter capacitiveFilter = CapacitiveFilter(DATA_FILTER_CAPACITANCE);
EepromCircularStack stack = EepromCircularStack(sizeof(uint32_t));
TM1637Display display(DISPLAY_CLOCK_PIN, DISPLAY_INPUT_PIN);

int16_t get_raw_value() {
  return analogRead(SENSOR_PIN);
}

bool is_line_problem(int16_t value) {
  return (value <= SENSOR_ABS_LOW) || (value >= SENSOR_ABS_HIGH);
}

int16_t get_humidity() {
  float result;

  int16_t raw_result = get_raw_value();
  int16_t display_result;

  if (is_line_problem(raw_result)) {
    result = -1;
    display_result = -raw_result;
  } else {
    capacitiveFilter.addValue(raw_result);
    result = capacitiveFilter.getValue();
    display_result = (int16_t)(result * 10);
    if (display_result >= 10000)
      display_result = 9999;
  }

  display.showNumberDec(display_result);

  /*
  Serial.print("get_humidity: ");
  Serial.print(result, 3);
  Serial.print(" ");
  Serial.print(raw_result);
  Serial.println("");
  //*/

  return result;
}

bool is_low_rh(int16_t cur_hum) {
  return
    (cur_hum != -1) &&
    (
      (SENSOR_HIGH_MEANS_DRY && (cur_hum > SENSOR_THRESHOLD)) ||
      (!SENSOR_HIGH_MEANS_DRY && (cur_hum < SENSOR_THRESHOLD))
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

bool ui32_gte(uint32_t a, uint32_t b) {
  return
    (a >= b) ||
    (
      (a < b) &&
      (b - a >= 0x40000000)
    );
}

float millis_to_hours(uint32_t num_millis) {
  return (float)num_millis / 1000 / 3600;
}

const uint8_t
  STATE_IDLE = 0,
  STATE_PRE_POUR_STAGING = 1,
  STATE_POURING = 2;

uint8_t state = STATE_IDLE;
uint32_t next_state_update_time = millis();

uint32_t last_motor_off_moment = 0;
uint32_t pour_duration_ms;

void loadPourSettings() {
  int16_t offset = stack.get_offs();
  if (offset >= 0) {
    EEPROM.get(offset, pour_duration_ms);
    constrainPourDuration();
  } else
    pour_duration_ms = BASE_POUR_DURATION_MS;
}

void savePourSettings() {
  int16_t offset = stack.add_offs();
  EEPROM.put(offset, pour_duration_ms);
}

void constrainPourDuration() {
  pour_duration_ms = min(pour_duration_ms, MAX_MOTOR_ON_TIME_MS);
}

const char
  CMD_GET_SETTINGS = 'S',
  CMD_GET_STATE = 'G',
  CMD_DUMP_EEPROM = 'D';

void print_usage() {
  String msg = "";
  msg =
    msg +
    "----------------------------------------------------\n" +
    "Code: \"" + code_name + "\"\n" +
    "Description: " + code_descr + "\n" +
    "Version: " + version + "\n" +
    "----------------------------------------------------\n" +
    "Usage:" + "\n" +
    "  " + CMD_GET_SETTINGS + " - Print current (s)ettings.\n" +
    "  " + CMD_GET_STATE + " - Get current status.\n" +
    "  " + CMD_DUMP_EEPROM + " - Dump EEPROM (stores <pour_duration_ms>).\n" +
    "\n";

  Serial.print(msg);
}

void print_settings() {
  String msg;
  // Avoid long (> 500 bytes) contents of <String>.

  msg = "";
  msg =
    msg +
    "Wiring:\n"
    "  Relay pin: " + RELAY_PIN + "\n" +
    "  Sensor pin: " + SENSOR_PIN + "\n" +
    "  Display input pin: " + DISPLAY_INPUT_PIN + "\n" +
    "  Display clock pin: " + DISPLAY_CLOCK_PIN + "\n" +
    "\n";
  Serial.print(msg);

  msg = "";
  msg =
    msg +
    "Settings:\n" +
    "  Sensor:\n" +
    "    Lowest valid value: " + SENSOR_ABS_LOW + "\n" +
    "    Highest valid value: " + SENSOR_ABS_HIGH + "\n" +
    "    Threshold value: " + SENSOR_THRESHOLD + "\n" +
    "    High values means dry?: " + SENSOR_HIGH_MEANS_DRY + "\n";
  Serial.print(msg);

  msg = "";
  msg =
    msg +
    "  Timings:\n" +
    "    Humidity refresh (s): " + GET_HUMIDITY_TICK_MS / 1000 + "\n" +
    "    Max motor ON time (s): " + MAX_MOTOR_ON_TIME_MS / 1000 + "\n" +
    "    IDLE state duration (s): " + IDLE_DURATION_MS / 1000 + "\n" +
    "    PRE_POUR state duration (s): " + PRE_POUR_DURATION_MS / 1000 + "\n" +
    "    Default POUR state duration (s): " + BASE_POUR_DURATION_MS / 1000 + "\n" +
    "    POST_POUR state duration (s): " + POST_POUR_DURATION_MS / 1000 + "\n" +
    "  Desired pour cycle (h): " + DESIRED_POUR_CYCLE_H + "\n";
    "\n";
  Serial.print(msg);
}

void print_status() {
  String msg = "";
  msg =
    msg +
    "Status:\n" +
    "  Humidity value: " + get_humidity() + "\n" +
    "  Pour duration (s): " + (float)pour_duration_ms / 1000 + "\n" +
    "  State: " + state + "\n" +
    "  Motor on?: " + motor.is_on() + "\n" +
    "  Uptime (h): " + millis_to_hours(millis()) + "\n" +
    "  Last motor off moment (h): " + millis_to_hours(last_motor_off_moment) + "\n" +
    "  Next state update moment (h): " + millis_to_hours(next_state_update_time) + "\n" +
    "\n";

  Serial.print(msg);
}

void handle_command() {
  char cmd = Serial.read();

  while (Serial.available())
    Serial.read();

  switch (cmd) {
    case CMD_GET_SETTINGS:
      print_settings();
      break;
    case CMD_GET_STATE:
      print_status();
      break;
    case CMD_DUMP_EEPROM:
      stack.dump();
      break;
    default:
      print_usage();
      break;
  }
}

void setup() {
  Serial.begin(9600);
  display.setBrightness(0x0F);
  loadPourSettings();
  print_usage();
}

void serialEvent() {
  handle_command();
}

void loop() {
  uint32_t cur_time = millis();
  int16_t cur_hum = get_humidity();

  bool is_time_to_update = ui32_gte(cur_time, next_state_update_time);
  if (is_time_to_update) {
    if (state == STATE_IDLE) {
      if (is_low_rh(cur_hum)) {
        state = STATE_PRE_POUR_STAGING;
        next_state_update_time = cur_time + PRE_POUR_DURATION_MS;
      } else {
        next_state_update_time = cur_time + IDLE_DURATION_MS;
      }
    } else if (state == STATE_PRE_POUR_STAGING) {
      if (is_low_rh(cur_hum)) {
        float numHoursPassed = millis_to_hours(cur_time - last_motor_off_moment);
        float correctionCoeff = DESIRED_POUR_CYCLE_H / numHoursPassed;
        if ((correctionCoeff < 1.0) || (last_motor_off_moment != 0)) {
          pour_duration_ms = pour_duration_ms * correctionCoeff;
          constrainPourDuration();
          savePourSettings();
        }
        state = STATE_POURING;
        next_state_update_time = cur_time + pour_duration_ms;
        motor_on();
      } else {
        state = STATE_IDLE;
        next_state_update_time = cur_time + IDLE_DURATION_MS;
      }
    } else if (state == STATE_POURING) {
      motor_off();
      last_motor_off_moment = cur_time;
      state = STATE_IDLE;
      next_state_update_time = cur_time + POST_POUR_DURATION_MS;
    }
  }

  delay(GET_HUMIDITY_TICK_MS);
}

/*
  2016-03-16
  [...]
  2020-03-14
    Reduced mostly experimental features of "Flower frined" to minimum.
  2020-11-14
    Reduced code even more.
  2020-12-28
    [+] CapacitiveFilter. Reduced code.
  2020-12-31
    [+] Dynamic calculation of pour duration.
*/
