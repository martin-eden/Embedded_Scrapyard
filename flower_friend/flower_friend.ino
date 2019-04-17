// Measures soil dryness and pours if needed.

/*
  Status: working
  Generation: 4.3
  Last mod.: 2019-04-13
*/

#include "humidity_measurer.h"
#include "switch.h"
#include <Wire.h>
#include "DateTime.h"
#include "me_ds3231.h"

const uint8_t
  pour_hours[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
  light_hours[24] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};

const int desired_rh_min = 80;
const int desired_rh_max = 85;

const int num_blocks = 2;

humidity_measurer measurer[num_blocks];
c_switch motor[num_blocks];
c_switch lamp;
me_ds3231 rtc;

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

const uint8_t
  motor_pins[num_blocks] = {2, 3},
  lamp_pin = 8;

void setup() {
  Serial.begin(9600);

  lamp.state_pin = lamp_pin;
  lamp.init();

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

  if (!rtc.isOscillatorAtBattery()) {
    Serial.println("Oscillator was disabled at battery mode. Enabling.");
    rtc.enableOscillatorAtBattery();
  }

  rtc.disableSqwAtBattery();

  /*
    Enable this block to set RTC to time at the moment of uploading
    sketch to board. Upload sketch. Disable block and upload sketch
    again.

    Constant added to time to compensate time between compilation
    on main computer and execution of setup() on board.
  */
  /*
  rtc.setDateTime(
    DateTime(F(__DATE__), F(__TIME__)) +
    TimeSpan(8)
  );
  rtc.clearOscillatorWasStopped();
  */

  loop();

  print_usage();
  print_status();
}

const uint32_t idle_measurement_delay = uint32_t(1000) * 60 * 12;
const uint32_t pour_measurement_delay = uint32_t(1000) * 5;
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

const char
  CMD_CLEAR_CLOCK_ERROR = 'C',
  CMD_MEASURE = 'M',
  CMD_MOTOR = 'T',
  CMD_GET_STATE = 'G';

void print_usage() {
  String msg = "";
  msg =
    msg +
    "Usage:" + "\n" +
    "  " + CMD_MEASURE + " <block_num>" + "\n" +
    "  " + "  " + "Measure probe <block_num>." + "\n" +
    "  " + CMD_MOTOR + " <block_num> (0 | 1)" + "\n" +
    "  " + "  " + "Enable/disable motor for given <block_num>." + "\n" +
    "  " + CMD_GET_STATE + "\n" +
    "  " + "  " + "Print current status." + "\n" +
    "  " + CMD_CLEAR_CLOCK_ERROR + "\n" +
    "  " + "  " + "Clear clock error flag." + "\n" +
    "\n";
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
    case CMD_MOTOR:
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
    case CMD_CLEAR_CLOCK_ERROR:
      Serial.read();
      rtc.clearOscillatorWasStopped();
      Serial.println("Clock-was-stopped flag is cleared.");
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
  DateTime rtc_time = rtc.getDateTime();
  result =
    result + rtc_time.year() +
    "-" + pad_zeroes(rtc_time.month()) +
    "-" + pad_zeroes(rtc_time.day()) +
    " " + pad_zeroes(rtc_time.hour()) +
    ":" + pad_zeroes(rtc_time.minute()) +
    ":" + pad_zeroes(rtc_time.second()) +
    // " " + "(day " + rtc_time.dayOfTheWeek() + ")" +
    "";
  return result;
}

String get_upload_time() {
  String result = "";
  DateTime upload_time = DateTime(F(__DATE__), F(__TIME__));
  result =
    result + upload_time.year() +
    "-" + pad_zeroes(upload_time.month()) +
    "-" + pad_zeroes(upload_time.day()) +
    " " + pad_zeroes(upload_time.hour()) +
    ":" + pad_zeroes(upload_time.minute()) +
    ":" + pad_zeroes(upload_time.second()) +
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

String get_light_hours() {
  String result = "";
  for (uint8_t i = 0; i < 24; i++) {
    if (light_hours[i]) {
      result = result + i + " ";
    }
  }
  return result;
}

// This function is from "ShowInfo" sketch.
float get_board_temp()
{
  unsigned int wADC;
  float result;

  // The internal temperature has to be used
  // with the internal reference of 1.1V.

  // This code is not valid for the Arduino Mega,
  // and the Arduino Mega 2560.

  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN); // enable the ADC

  delay(20); // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC); // Start the ADC

  while (bit_is_set(ADCSRA,ADSC));

  wADC = ADCW;

  // The offset of 337.0 could be wrong. It is just an indication.
  result = (wADC - 337.0 ) / 1.22;

  return result;
}

String represent_time_passed(uint32_t seconds) {
  String result = "";
  TimeSpan time_span = seconds;
  bool construction_started = false;

  if (time_span.days() > 0) {
    result = result + time_span.days() + "d ";
    construction_started = true;
  }
  if (construction_started || (time_span.hours() > 0)) {
    result = result + time_span.hours() + "h ";
    construction_started = true;
  }
  if (construction_started || (time_span.minutes() > 0)) {
    result = result + time_span.minutes() + "m ";
    construction_started = true;
  }
  result = result + time_span.seconds() + "s";

  return result;
}

uint32_t cur_time;

void print_status() {
  String msg;

  msg = "";
  msg =
    msg +
    "Status:" + "\n" +
    "  " + "Pour settings:" + "\n" +
    "  " + "  " + "light_hours: " + get_light_hours() + "\n" +
    "  " + "  " + "pour_hours: " + get_pour_hours() + "\n" +
    "  " + "  " + "desired_rh_min: " + desired_rh_min + "\n" +
    "  " + "  " + "desired_rh_max: " + desired_rh_max + "\n";
  Serial.print(msg);


  msg = "";
  msg =
    msg +
    "  " + "Time:" + "\n" +
    "  " + "  " + "rtc_time: " + get_rtc_time() + "\n" +
    "  " + "  " + "rtc_status: ";
  if (rtc.oscillatorWasStopped())
    msg = msg + "Clock was stopped. Battery is over?";
  else
    msg = msg + "ok";
  msg = msg + "\n";
  Serial.print(msg);

  msg = "";
  msg =
    msg +
    "  " + "  " + "upload_time: " + get_upload_time() + "\n" +
    "";
  Serial.print(msg);

  Serial.print("    uptime: ");
  Serial.print(represent_time_passed(cur_time / 1000));
  Serial.print("\n");

  Serial.print("    board_temperature: ");
  Serial.print(get_board_temp(), 2);
  Serial.print("\n");

  Serial.print("    rtc_temperature: ");
  Serial.print(rtc.getTemp(), 2);
  Serial.print("\n");

  Serial.println("  Delays:");

  Serial.print("    idle_measurement_delay: ");
  Serial.print((float)idle_measurement_delay / 1000);
  Serial.print("\n");
  Serial.print("    pour_measurement_delay: ");
  Serial.print((float)pour_measurement_delay / 1000);
  Serial.print("\n");

  uint8_t lamp_is_on = lamp.is_on;
  msg = "";
  msg = msg + "  Lamp: " + lamp_is_on + "\n";
  Serial.print(msg);

  Serial.println("  Blocks:");
  for (int i = 0; i < num_blocks; i++) {
    /*
      <is_line_problem> is set inside <get_value()>.
      In string constructor "a() + b()" the actual call order
        is b(), a().
      So we force correct order via direct assignments.
    */
    uint8_t value = measurer[i].get_value();
    uint8_t is_line_problem = measurer[i].is_line_problem; //
    msg = "";
    msg =
      msg +
      "  " + "  " + "block[" + i + "]:" + "\n" +
      "  " + "  " + "  " +
      "sensor: " + value + ", " +
      "is_line_problem: " + is_line_problem + ", " +
      "motor: " + motor[i].is_on + "\n";
    Serial.print(msg);
  }

  Serial.print("\n");
}

void do_common_business(DateTime rtc_time)
{
  if (light_hours[rtc_time.hour()] && !lamp.is_on)
    lamp.switch_on();
  if (!light_hours[rtc_time.hour()] && lamp.is_on)
    lamp.switch_off();
}

void do_block_business(DateTime rtc_time, uint8_t block_num)
{
  if (
    (cur_time < next_request_time[block_num]) ||
    (
      (cur_time >= 0x80000000) && (next_request_time[block_num] < 0x80000000)
    )
  )
    return;

  if (pour_hours[rtc_time.hour()] || motor[block_num].is_on) {
    int val = measurer[block_num].get_value();
    if (measurer[block_num].is_line_problem) {
      motor[block_num].switch_off();
    }
    else {
      if ((val <= desired_rh_min) && !motor[block_num].is_on) {
        motor[block_num].switch_on();
        // print_status();
      }
      if ((val >= desired_rh_max) && (motor[block_num].is_on)) {
        motor[block_num].switch_off();
        // print_status();
      }
    }
  }

  if (motor[block_num].is_on)
    next_request_time[block_num] = cur_time + pour_measurement_delay;
  else
    next_request_time[block_num] = cur_time + idle_measurement_delay;
}

void do_business()
{
  DateTime rtc_time;
  bool time_to_work = false;

  for (uint8_t block_num = 0; block_num < num_blocks; block_num++)
  {
    if (
      (cur_time >= next_request_time[block_num]) ||
      (
        (cur_time < 0x80000000) && (next_request_time[block_num] >= 0x80000000)
      )
    )
      time_to_work = true;
  }

  if (!time_to_work)
    return;

  rtc_time = rtc.getDateTime();

  do_common_business(rtc_time);

  for (uint8_t block_num = 0; block_num < num_blocks; block_num++)
    do_block_business(rtc_time, block_num);
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
  2018-12-04
  2019-01-12
  2019-01-18
    Correct RTC usage.
  2019-01-29
  2019-02-27
  2019-03-21
  2019-03-30
    Lamp support.
*/
