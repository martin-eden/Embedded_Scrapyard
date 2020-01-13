// Prototype of logging data to SD card from DHT and RTC.

/*
  Status: works
  Version: 1.0
  Last mod.: 2020-01-13
*/

#include <SPI.h>
#include <SD.h>
#include <SimpleDHT.h>

#include "me_ds3231.h"
#include "me_DateTime.h"

const uint8_t
  SDCARD_CS_PIN = 4,
  DHT_PIN = 2;

const uint32_t
  DELAY_SECS = 6 * 60;

SimpleDHT11 dht11(DHT_PIN);
me_ds3231 ds3231 = me_ds3231();

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  write_string(String("% Initialized."));
}

String get_data()
{
  String result = "";
  DateTime dt = ds3231.getDateTime();
  const uint8_t MAX_MSG_LEN = 2 * 16;
  char msg_buf[MAX_MSG_LEN];
  float temperature = 0;
  float humidity = 0;

  int16_t read_error = dht11.read2(&temperature, &humidity, NULL);
  if (read_error)
  {
    Serial.print("Read DHT11 failed, err="); Serial.println(read_error);
  }
  else
  {
    dt.represent_date(msg_buf, MAX_MSG_LEN);
    result += msg_buf;
    dt.represent_time(msg_buf, MAX_MSG_LEN);
    result += " ";
    result += msg_buf;
    result += " ";
    result += String(temperature, 2);
    result += " ";
    result += String(humidity, 2);
  }

  return result;
}

void write_string(String s)
{
  if (!SD.begin(SDCARD_CS_PIN))
  {
    Serial.println("SD card failed, or not present.");
    return;
  }

  File data_file = SD.open("datalog.txt", FILE_WRITE);

  if (data_file)
  {
    data_file.println(s);
    data_file.close();
    Serial.println(s);
  }
  else
  {
    Serial.println("Error opening 'datalog.txt'.");
  }
}

void loop()
{
  String data_string = get_data();
  write_string(data_string);
  delay(DELAY_SECS * 1000);
}
