/*
  This code is not mine!

  I've copied it from some RTC DS3231 module library in 2019 with
  intention to strip and adjust code to my needs. At that time
  "Sketchbook/" was just internal directory, not a repo.

  So sorry for that.

  -- Martin, 2024-05-07
*/

#pragma once

#include <Arduino.h>

class TimeSpan;

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime {
  protected:
    uint8_t yOff, m, d, hh, mm, ss;
  public:
    DateTime(uint32_t t = 0);
    DateTime(
      uint16_t year,
      uint8_t month,
      uint8_t day,
      uint8_t hour = 0,
      uint8_t min = 0,
      uint8_t sec = 0
    );
    DateTime(const DateTime& copy);
    DateTime(const char* date, const char* time);
    DateTime(const __FlashStringHelper* date, const __FlashStringHelper* time);

    uint16_t year() const {
      return 2000 + yOff;
    }
    uint8_t month() const {
      return m;
    }
    uint8_t day() const {
      return d;
    }
    uint8_t hour() const {
      return hh;
    }
    uint8_t minute() const {
      return mm;
    }
    uint8_t second() const {
      return ss;
    }
    uint8_t dow() const;

    void represent_date(char* pszResult, uint8_t capacity) const;
    void represent_time(char* pszResult, uint8_t capacity) const;
    String representDateTime();
    void represent_dow(char* pszResult, uint8_t capacity) const;
    // void represent(char* pszResult, uint8_t capacity) const; // -- need debugging

    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const;

    DateTime operator + (const TimeSpan& span);
    DateTime operator - (const TimeSpan& span);
    TimeSpan operator - (const DateTime& right);
};

// Timespan which can represent changes in time with seconds accuracy.
class TimeSpan {
  protected:
    int32_t _seconds;
  public:
    TimeSpan(int32_t seconds = 0);
    TimeSpan(
      int16_t days,
      int8_t hours,
      int8_t minutes,
      int8_t seconds
    );
    TimeSpan(const TimeSpan& copy);
    int16_t days() const {
      return _seconds / 86400L;
    }
    int8_t hours() const {
      return _seconds / 3600 % 24;
    }
    int8_t minutes() const {
      return _seconds / 60 % 60;
    }
    int8_t seconds() const {
      return _seconds % 60;
    }
    int32_t totalseconds() const {
      return _seconds;
    }

    TimeSpan operator + (const TimeSpan& right);
    TimeSpan operator - (const TimeSpan& right);
};
