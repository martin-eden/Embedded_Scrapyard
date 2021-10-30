// Set specific date-time for DS3231 RTC.

/*
  Status: in progress
  Version: 
  Last mod.: 2021-10-04
*/

#include "me_ds3231.h"
#include "me_DateTime.h"

me_ds3231 rtc = me_ds3231();

void setup() {
  // DateTime dt = (2021, 10, 04, 13, 11, 00);
  // rtc.setDateTime(dt);
  
  Serial.begin(9600);

  Serial.println("dt.representDateTime()");
  delay(200);
  DateTime dt = rtc.getDateTime();
  delay(200);
  Serial.println(dt.year());
  Serial.println("hmm");
  // Serial.println(dt.representDateTime());
}

void loop() {	
}