#include "me_DataLogger.h"

#include <SD.h>

me_DataLogger::me_DataLogger(uint8_t a_cs_pin)
{
  cs_pin = a_cs_pin;
}

bool me_DataLogger::writeString(String s)
{
  // Serial.println(s);

  if (!SD.begin(cs_pin))
  {
    Serial.println("SD card failed, or not present.");
    return false;
  }

  File data_file = SD.open("datalog.txt", FILE_WRITE);

  if (data_file)
  {
    data_file.println(s);
    data_file.close();
  }
  else
  {
    Serial.println("Error opening 'datalog.txt'.");
    return false;
  }

  SD.end();
  return true;
}
