// Measures soil dryness and pours if needed.

/*
  Status: stable
  Generation: 0.7
  Last mod.: 2020-03-14
*/

#include <me_humidity_measurer.h>
#include <me_switch.h>
#include <me_DateTime.h>

const char
  code_descr[] = "\"Pour manager\". Based on \"Flower friend\" gardening system.",
  version[] = "0.7.2";

const uint8_t
  PIN_MEASURER_SIGNAL = A0,
  PIN_MEASURER_POWER = 8,
  PIN_MOTOR_CONTROL = 2,

  DESIRED_RH_FROM = 40,
  DESIRED_RH_TO = 80;

const uint16_t
  MEASURER_MIN_VALUE = 460,
  MEASURER_MAX_VALUE = 600,
  MEASURER_HYSTERESIS = 10;

const bool
  MEASURER_HIGH_MEANS_DRY = false;

const uint32_t
  IDLE_MEASUREMENT_DELAY = uint32_t(1000) * 60 * 12,
  POUR_MEASUREMENT_DELAY = uint32_t(1000) * 5;

c_humidity_measurer measurer;
c_switch motor = c_switch(PIN_MOTOR_CONTROL);

struct t_measurer_params
  {
    uint8_t power_pin;
    uint8_t sensor_pin;
    int16_t min_value;
    int16_t max_value;
    uint16_t hysteresis;
    bool power_off_between_measures;
    bool high_means_dry;
  };

const t_measurer_params sensor_params =
  {
    power_pin: PIN_MEASURER_POWER,
    sensor_pin: PIN_MEASURER_SIGNAL,
    min_value: MEASURER_MIN_VALUE,
    max_value: MEASURER_MAX_VALUE,
    hysteresis: MEASURER_HYSTERESIS,
    power_off_between_measures: true,
    high_means_dry: MEASURER_HIGH_MEANS_DRY
  };

void setup() {
  Serial.begin(9600);

  Serial.println("Setup. Motors.");
  Serial.println("Setup. Sensors.");
  init_moisture_sensors();

  // assure normal business logic was done before printing status:
  Serial.println("Setup. Loop.");
  loop();

  print_signature();
  print_usage();
  print_status();
}

void init_moisture_sensors() {
  if (sensor_params.sensor_pin != -1) {
    pinMode(sensor_params.sensor_pin, INPUT);
    measurer.sensor_pin = sensor_params.sensor_pin;
  }
  if (sensor_params.min_value != -1)
    measurer.min_value = sensor_params.min_value;
  if (sensor_params.max_value != -1)
    measurer.max_value = sensor_params.max_value;
  measurer.hysteresis = sensor_params.hysteresis;
  measurer.power_off_between_measures = sensor_params.power_off_between_measures;
  measurer.high_means_dry = sensor_params.high_means_dry;
  if (sensor_params.power_pin != -1) {
    pinMode(sensor_params.power_pin, OUTPUT);
    measurer.power_pin = sensor_params.power_pin;
  }
}

uint32_t next_request_time;

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

void print_signature() {
  Serial.println("-----------------------------------");

  Serial.print(code_descr);
  Serial.println();

  Serial.print("  Version: ");
  Serial.print(version);
  Serial.println();

  Serial.print("  Uploaded: ");
  Serial.print(get_upload_time());
  Serial.println();

  Serial.print("  File: ");
  Serial.print(F(__FILE__));
  Serial.println();

  Serial.println("-----------------------------------");
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
    "  " + CMD_MEASURE + "\n" +
    "  " + "  " + "Measure probe." + "\n" +
    "  " + CMD_MOTOR + " (0 | 1)" + "\n" +
    "  " + "  " + "Enable/disable motor." + "\n" +
    "  " + CMD_GET_STATE + "\n" +
    "  " + "  " + "Print current status." + "\n" +
    "\n";
  Serial.print(msg);
}

void handle_command() {
  char cmd = Serial.peek();
  uint8_t data_length = Serial.available();
  int8_t on_off;
  uint8_t value;
  switch (cmd) {
    case CMD_MEASURE:
      Serial.read();
      value = measurer.get_value();
      send_measurement(value);
      break;
    case CMD_MOTOR:
      if (data_length < 2)
        break;
      Serial.read();
      on_off = parse_on_off(Serial.read());
      if (on_off < 0)
        break;
      if (on_off == 1)
        motor.switch_on();
      else
        motor.switch_off();
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

String get_upload_time()
{
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
    "  Pour settings:" + "\n" +
    "    MEASURER_MIN_VALUE: " + MEASURER_MIN_VALUE + "\n" +
    "    MEASURER_MAX_VALUE: " + MEASURER_MAX_VALUE + "\n" +
    "    MEASURER_HYSTERESIS: " + MEASURER_HYSTERESIS + "\n" +
    "    DESIRED_RH_FROM: " + DESIRED_RH_FROM + "\n" +
    "    DESIRED_RH_TO: " + DESIRED_RH_TO + "\n";
  Serial.print(msg);

  Serial.print("    uptime: ");
  Serial.print(represent_time_passed(cur_time / 1000));
  Serial.print("\n");

  Serial.println("  Delays:");

  Serial.print("    IDLE_MEASUREMENT_DELAY: ");
  Serial.print((float)IDLE_MEASUREMENT_DELAY / 1000);
  Serial.print("\n");
  Serial.print("    POUR_MEASUREMENT_DELAY: ");
  Serial.print((float)POUR_MEASUREMENT_DELAY / 1000);
  Serial.print("\n");

  Serial.println("  Block:");
  uint8_t value = measurer.get_value();
  uint16_t raw_value = measurer.get_raw_value();
  uint8_t is_line_problem = measurer.is_line_problem; // (1)
  msg = "";
  msg =
    msg +
    "  " + "  " +
    "moisture_humidity_%: " + value + ", " +
    "raw_value: " + raw_value + ", " +
    "is_line_problem: " + is_line_problem + ", " +
    "motor: " + motor.is_on() + "\n";
  Serial.print(msg);

  Serial.print("\n");
  /*
    [1]:
      <is_line_problem> is set inside <get_value()>. In string
      constructor "a() + b()" the actual call order is b(), a().
      So we force correct order via direct assignments.
  */
}

bool is_time_to_work() {
  return
    (cur_time >= next_request_time) ||
    (
      (cur_time < 0x10000000) &&
      (next_request_time >= 0x80000000)
    );
}

void do_business() {
  bool time_to_work = is_time_to_work();

  if (!time_to_work)
    return;

  int val = measurer.get_value();
  if (measurer.is_line_problem) {
    motor.switch_off();
    print_status();
  }
  else {
    if ((val <= DESIRED_RH_FROM) && motor.is_off()) {
      motor.switch_on();
      print_status();
    }
    else if ((val >= DESIRED_RH_TO) && motor.is_on()) {
      motor.switch_off();
      print_status();
    }
  }

  if (motor.is_on())
    next_request_time = cur_time + POUR_MEASUREMENT_DELAY;
  else
    next_request_time = cur_time + IDLE_MEASUREMENT_DELAY;
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
*/
