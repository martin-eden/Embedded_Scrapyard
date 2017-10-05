// Measures soil dryness and pours if needed.

/*
  Status: working
  Generation: 4.0.2
  Last mod.: 2017-10-05
*/

#include "humidity_measurer.h"
#include "switch.h"
#include <Wire.h>
#include "RTClib.h"

const uint8_t pour_hours[24] =
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};

const int num_blocks = 2;

humidity_measurer measurer[num_blocks];
c_switch motor[num_blocks];
RTC_DS3231 rtc;

struct t_measurer_params {
  int sensor_pin;
  int min_value;
  int max_value;
  bool power_off_between_measures;
  bool high_means_dry;
  int power_pin;
};

const t_measurer_params sensor_params[num_blocks] =
  {
    {A1, -1, -1, true, false, 6},
    {A0, -1, -1, true, false, 7}
  };

const int motor_pins[num_blocks] = {2, 3};

uint32_t cur_time;
uint32_t last_print_time;
DateTime rtc_time;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < num_blocks; i++) {
    motor[i].state_pin = motor_pins[i];
    motor[i].init();
  }

  for (int i = 0; i < num_blocks; i++) {
    if (sensor_params[i].sensor_pin != -1) {
      pinMode(sensor_params[i].sensor_pin, INPUT);
      measurer[i].sensor_pin = sensor_params[i].sensor_pin;
    }
    if (sensor_params[i].min_value != -1)
      measurer[i].min_value = sensor_params[i].min_value;
    if (sensor_params[i].max_value != -1)
      measurer[i].max_value = sensor_params[i].max_value;
    measurer[i].power_off_between_measures = sensor_params[i].power_off_between_measures;
    measurer[i].high_means_dry = sensor_params[i].high_means_dry;
    if (sensor_params[i].power_pin != -1) {
      pinMode(sensor_params[i].power_pin, OUTPUT);
      measurer[i].power_pin = sensor_params[i].power_pin;
    }
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC.");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting current time.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  last_print_time = millis();
}

unsigned long min_debug_message_gap_ms = 100;

const int pour_on_percent = 70;
const int pour_off_percent = 80;
const uint32_t idle_measurement_delay = uint32_t(60) * 12 * 1000;
const uint32_t pour_measurement_delay = uint32_t(5) * 1000;
uint32_t next_request_time[num_blocks];

int parse_block_num(char c) {
  if (c == '0')
    return 0;
  else if (c == '1')
    return 1;
  else
    return -1;
}

int parse_on_off(char c) {
  if (c == '0')
    return 0;
  else if (c == '1')
    return 1;
  else
    return -1;
}

void send_measurement(uint8_t value) {
  String msg = "";
  msg = msg + "value: " + value;
  Serial.println(msg);
}

const char CMD_MEASURE = 'M';
const char CMD_EXECUTE = 'X';
const char CMD_GET_STATE = 'G';

void print_usage() {
  String msg = "";
  msg =
    msg +
    "--" + "\n" +
    "Usage:" + "\n" +
    "  " + CMD_MEASURE + " <block_num>" + "\n" +
    "    Measure probe <block_num>." + "\n" +
    "  " + CMD_EXECUTE + " <block_num> (0 | 1)" + "\n" +
    "    Motor on/off for given <block_num>." + "\n" +
    "  " + CMD_GET_STATE + " " + "\n" +
    "    Print current status." + "\n";
  Serial.print(msg);
}

void handle_command() {
  char cmd = Serial.peek();
  uint8_t data_length = Serial.available();
  int8_t block_num;
  int8_t on_off;
  uint8_t value;
  switch (cmd) {
    case CMD_MEASURE:
      if (data_length < 2)
        break;
      Serial.read();
      block_num = parse_block_num(Serial.read());
      if (block_num < 0)
        break;
      value = measurer[block_num].get_value();
      send_measurement(value);
      break;
    case CMD_EXECUTE:
      if (data_length < 3)
        break;
      Serial.read();
      block_num = parse_block_num(Serial.read());
      on_off = parse_on_off(Serial.read());
      if ((block_num < 0) || (on_off < 0))
        break;
      if (on_off == 1)
        motor[block_num].switch_on();
      else
        motor[block_num].switch_off();
      break;
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

String pad_zeroes(uint8_t value) {
  String result = "";
  if (value < 10)
    result = result + "0";
  result = result + value;
  return result;
}

String get_rtc_time() {
  String result = "";
  result =
    result +
    // rtc_time.year() + "-" +
    // pad_zeroes(rtc_time.month()) + "-" +
    // pad_zeroes(rtc_time.day()) + " " +
    pad_zeroes(rtc_time.hour()) + ":" +
    pad_zeroes(rtc_time.minute()) + ":" +
    pad_zeroes(rtc_time.second()) + " " +
    // "(day " + rtc_time.dayOfTheWeek() + ")" +
    "";
  return result;
}

String get_pour_hours() {
  String result = "";
  for (uint8_t i = 0; i < 24; i++) {
    if (pour_hours[i]) {
      result = result + i + " ";
    }
  }
  return result;
}

void print_status() {
  String msg = "";
  if (cur_time - last_print_time > min_debug_message_gap_ms) {
    String msg;

    msg = "";
    msg =
      msg +
      "--" + "\n" +
      "rtc_time: " + get_rtc_time() + "\n" +
      "pour_hours: " + get_pour_hours() + "\n" +
      "pour_on_percent: " + pour_on_percent + ", " +
      "pour_off_percent: " + pour_off_percent + "\n";
    Serial.print(msg);

    Serial.print("idle_measurement_delay: ");
    Serial.print((float)idle_measurement_delay / 1000);
    Serial.print(", ");
    Serial.print("pour_measurement_delay: ");
    Serial.print((float)pour_measurement_delay / 1000);
    Serial.println("");
    Serial.print("uptime_secs: ");
    Serial.print((float)cur_time / 1000);
    Serial.println("");

    for (int i = 0; i < num_blocks; i++) {
      /*
        <is_line_problem> is actual after <get_value()>.
        In string constructor "a() + b()" actual order is b(), a().
        So we force correct order via direct assignments.
      */
      uint8_t value = measurer[i].get_value();
      uint8_t is_line_problem = measurer[i].is_line_problem; //
      msg = "";
      msg =
        msg +
        "block " + i + ":" + "\n" +
        "  " +
        "sensor: " + value + ", " +
        "is_line_problem: " + is_line_problem + ", " +
        "motor: " + motor[i].is_on + "\n";
      Serial.print(msg);
    }
  }

  last_print_time = cur_time;
}

void do_business() {
  for (int block = 0; block < num_blocks; block++) {
    if (
      (cur_time >= next_request_time[block]) ||
      (
        (cur_time < 0x80000000) &&
        (next_request_time[block] >= 0x80000000)
      )
    ) {
      if (pour_hours[rtc_time.hour()] || motor[block].is_on) {
        int val = measurer[block].get_value();
        if (measurer[block].is_line_problem) {
          motor[block].switch_off();
        }
        else {
          if ((val <= pour_on_percent) && !motor[block].is_on) {
            motor[block].switch_on();
            // print_status();
          }
          if ((val >= pour_off_percent) && (motor[block].is_on)) {
            motor[block].switch_off();
            // print_status();
          }
        }

        if (motor[block].is_on)
          next_request_time[block] = cur_time + pour_measurement_delay;
        else
          next_request_time[block] = cur_time + idle_measurement_delay;
      }
    }
  }
}

void serialEvent() {
  handle_command();
}

void loop() {
  cur_time = millis();
  rtc_time = rtc.now();
  do_business();
}

/*
  2016-03-16
  2016-03-30
  2016-03-31
  2016-04-21
  2016-04-28
  2016-05-27
  2016-10-11
  2017-01-05
  2017-03-24
  2017-06-30
    RTC module added.
  2017-07-16
  2017-10-05
*/
