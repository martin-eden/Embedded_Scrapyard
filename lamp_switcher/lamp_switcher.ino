// Measures soil dryness and pours if needed.

/*
  Status: stable
  Generation: 4.6.2
  Last mod.: 2019-05-28
*/

#include "switch.h"
#include <Wire.h>
#include "DateTime.h"
#include "me_ds3231.h"

String
  code_descr = "\"Lamp switcher\" (trucated \"Flower friend\") gardening system",
  version = "1.0.2";

struct t_suntime
  {
    uint8_t sunrise;
    uint8_t sunset;
  };

const t_suntime
  sun_month[12] =
    {
      {8, 17},
      {7, 17},
      {6, 18},
      {5, 20},
      {4, 22},
      {4, 22},
      {4, 22},
      {4, 20},
      {6, 19},
      {7, 18},
      {7, 17},
      {8, 17},
    };

c_switch lamp;
me_ds3231 rtc;

const uint8_t
  lamp_pin = 6;

void setup()
{
  Serial.begin(9600);

  init_lamp();
  init_clock();
  // setup_clock();

  // assure normal business logic was done before printing status:
  loop();

  print_signature();
  print_usage();
  print_status();
}

void init_lamp()
{
  lamp.state_pin = lamp_pin;
  lamp.init();
}

void init_clock()
{
  if (!rtc.isOscillatorAtBattery())
  {
    Serial.println("Oscillator was disabled at battery mode. Enabling.");
    rtc.enableOscillatorAtBattery();
  }
  rtc.disableSqwAtBattery();
  rtc.disable32kWave();
  // rtc.setDateTime(DateTime(2089, 5, 1));
}

void setup_clock()
{
  /*
    Call this function from setup() to set RTC to time to the moment
    of uploading sketch to board. Upload sketch. Disable this function
    call and upload sketch again.

    Constant added to time to compensate time between compilation
    on main computer and execution of setup() on board.
  */

  uint8_t time_from_compile_to_setup_secs = 8;

  rtc.setDateTime(
    DateTime(F(__DATE__), F(__TIME__)) +
    TimeSpan(time_from_compile_to_setup_secs)
  );
  rtc.clearOscillatorWasStopped();
}

const uint32_t idle_measurement_delay = 1000;
uint32_t next_request_time;

void print_signature()
{
  Serial.println("-----------------------------------");

  Serial.print(code_descr);
  Serial.println();

  Serial.print("  Version: ");
  Serial.print(version);
  Serial.println();

  Serial.print("  Compiled: ");
  Serial.print(get_compile_time());
  Serial.println();

  Serial.print("  File: ");
  Serial.print(F(__FILE__));
  Serial.println();

  Serial.println("-----------------------------------");
}

const char
  CMD_CLEAR_CLOCK_ERROR = 'C',
  CMD_GET_STATE = 'G';

void print_usage()
{
  String msg = "";
  msg =
    msg +
    "Usage:" + "\n" +
    "  " + CMD_GET_STATE + "\n" +
    "  " + "  " + "Print current status." + "\n" +
    "  " + CMD_CLEAR_CLOCK_ERROR + "\n" +
    "  " + "  " + "Clear clock error flag." + "\n" +
    "\n";
  Serial.print(msg);
}

void handle_command()
{
  char cmd = Serial.peek();
  switch (cmd)
  {
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
      Serial.print("Unknown command: 0x");
      Serial.println(cmd, HEX);
      Serial.read();
      print_usage();
      break;
  }
}

String pad_zeroes(uint8_t value)
{
  String result = "";
  if (value < 10)
    result = result + "0";
  result = result + value;
  return result;
}

DateTime rtc_time;

String get_rtc_time()
{
  String result = "";
  rtc_time = rtc.getDateTime();
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

String get_compile_time()
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

String get_light_hours(uint8_t month)
{
  String result = "";
  result =
    result +
    sun_month[month - 1].sunrise + ".." + sun_month[month - 1].sunset;
  return result;
}

/*
  This function is from "ShowInfo" sketch.

  I've changed constants to better fit my case.
*/
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

  while (bit_is_set(ADCSRA, ADSC));

  wADC = ADCW;

  // The constants could be wrong. It is just an adjustment.
  result = (wADC - 328.0 ) / 1.04;

  return result;
}

String represent_time_passed(uint32_t seconds)
{
  String result = "";
  TimeSpan time_span = seconds;
  bool construction_started = false;

  if (time_span.days() > 0)
  {
    result = result + time_span.days() + "d ";
    construction_started = true;
  }
  if (construction_started || (time_span.hours() > 0))
  {
    result = result + time_span.hours() + "h ";
    construction_started = true;
  }
  if (construction_started || (time_span.minutes() > 0))
  {
    result = result + time_span.minutes() + "m ";
    construction_started = true;
  }
  result = result + time_span.seconds() + "s";

  return result;
}

uint32_t cur_time;

void print_status()
{
  String msg;

  msg = "";
  msg =
    msg +
    "Status:" + "\n" +
    "  " + "Gardening settings:" + "\n" +
    "  " + "  " + "light_hours: " + get_light_hours(rtc_time.month()) + "\n" +
    "";
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

  Serial.print("    uptime: ");
  Serial.print(represent_time_passed(cur_time / 1000));
  Serial.print("\n");

  Serial.print("    board_temperature: ");
  Serial.print(get_board_temp(), 2);
  Serial.print("\n");

  Serial.print("    rtc_temperature: ");
  Serial.print(rtc.getTemp(), 2);
  Serial.print("\n");

  Serial.print("  Delays:\n");

  Serial.print("    idle_measurement_delay: ");
  Serial.print((float)idle_measurement_delay / 1000);
  Serial.print("\n");

  uint8_t lamp_is_on = lamp.is_on;
  msg = "";
  msg = msg + "  Lamp: " + lamp_is_on + "\n";
  Serial.print(msg);

  Serial.print("\n");
}

void do_common_business()
{
  uint8_t
    sunrise = sun_month[rtc_time.month() - 1].sunrise,
    sunset = sun_month[rtc_time.month() - 1].sunset,
    hour = rtc_time.hour();
  if (!lamp.is_on && (hour >= sunrise) && (hour < sunset))
    lamp.switch_on();
  if (lamp.is_on && ((hour < sunrise) || (hour >= sunset)))
    lamp.switch_off();
}

void do_business()
{
  bool time_to_work = false;

  if (
    (cur_time >= next_request_time) ||
    (
      (cur_time < 0x10000000) && (next_request_time >= 0x80000000)
    )
  )
    time_to_work = true;

  if (!time_to_work)
    return;

  rtc_time = rtc.getDateTime();

  do_common_business();

  next_request_time += idle_measurement_delay;
}

void serialEvent()
{
  handle_command();
}

void loop()
{
  cur_time = millis();
  do_business();
}

/*
  2019-05-05
    Prined [flower_friend] to lamp support only.
*/
