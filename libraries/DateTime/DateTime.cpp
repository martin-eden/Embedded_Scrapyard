#include "DateTime.h"
#include <assert.h>

const uint32_t seconds_from_1970_to_2000 = 946684800;

const uint8_t days_in_month [] PROGMEM =
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t get_num_days(
  uint16_t year,
  uint8_t month,
  uint8_t day
)
{
  assert((year >= 2000) && (year <= 2099));
  year -= 2000;
  uint16_t days = day;
  for (uint8_t i = 1; i < month; ++i)
    days += pgm_read_byte(days_in_month + i - 1);
  if (month > 2 && year % 4 == 0)
    ++days;
  return days + 365 * year + (year + 3) / 4 - 1;
}

static uint32_t get_num_secs(
  uint16_t days,
  uint8_t hour,
  uint8_t minute,
  uint8_t second
)
{
    return ((days * 24L + hour) * 60 + minute) * 60 + second;
}

DateTime::DateTime(
  uint32_t t
)
{
  t -= seconds_from_1970_to_2000;

  ss = t % 60;
  t /= 60;
  mm = t % 60;
  t /= 60;
  hh = t % 24;
  uint16_t days = t / 24;
  uint8_t leap;
  for (yOff = 0; ; ++yOff) {
    leap = (yOff % 4 == 0);
    if (days < 365 + leap)
      break;
    days -= 365 + leap;
  }
  for (m = 1; ; ++m) {
    uint8_t days_in_this_month = pgm_read_byte(days_in_month + m - 1);
    if (leap && m == 2)
      ++days_in_this_month;
    if (days < days_in_this_month)
      break;
    days -= days_in_this_month;
  }
  d = days + 1;
}

DateTime::DateTime(
  uint16_t year,
  uint8_t month,
  uint8_t day,
  uint8_t hour,
  uint8_t min,
  uint8_t sec
)
{
  assert((year >= 2000) && (year <= 2099));
  year -= 2000;
  yOff = year;
  m = month;
  d = day;
  hh = hour;
  mm = min;
  ss = sec;
}

DateTime::DateTime(const DateTime& copy):
  yOff(copy.yOff),
  m(copy.m),
  d(copy.d),
  hh(copy.hh),
  mm(copy.mm),
  ss(copy.ss)
{}

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

// To do: rewrite this garbage for ISO8601 date-times.

// A convenient constructor for using "the compiler's time":
//   DateTime now (__DATE__, __TIME__);
// NOTE: using F() would further reduce the RAM footprint, see below.
DateTime::DateTime (const char* date, const char* time) {
  // sample input: date = "Dec 26 2009", time = "12:34:56"
  yOff = conv2d(date + 9);
  // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
  switch (date[0]) {
    case 'J': m = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7); break;
    case 'F': m = 2; break;
    case 'A': m = date[2] == 'r' ? 4 : 8; break;
    case 'M': m = date[2] == 'r' ? 3 : 5; break;
    case 'S': m = 9; break;
    case 'O': m = 10; break;
    case 'N': m = 11; break;
    case 'D': m = 12; break;
  }
  d = conv2d(date + 4);
  hh = conv2d(time);
  mm = conv2d(time + 3);
  ss = conv2d(time + 6);
}

// A convenient constructor for using "the compiler's time":
// This version will save RAM by using PROGMEM to store it by using the F macro.
//   DateTime now (F(__DATE__), F(__TIME__));
DateTime::DateTime (const __FlashStringHelper* date, const __FlashStringHelper* time) {
  // sample input: date = "Dec 26 2009", time = "12:34:56"
  char buff[11];
  memcpy_P(buff, date, 11);
  yOff = conv2d(buff + 9);
  // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
  switch (buff[0]) {
    case 'J': m = (buff[1] == 'a') ? 1 : ((buff[2] == 'n') ? 6 : 7); break;
    case 'F': m = 2; break;
    case 'A': m = buff[2] == 'r' ? 4 : 8; break;
    case 'M': m = buff[2] == 'r' ? 3 : 5; break;
    case 'S': m = 9; break;
    case 'O': m = 10; break;
    case 'N': m = 11; break;
    case 'D': m = 12; break;
  }
  d = conv2d(buff + 4);
  memcpy_P(buff, time, 8);
  hh = conv2d(buff);
  mm = conv2d(buff + 3);
  ss = conv2d(buff + 6);
}

uint8_t DateTime::dow() const {
  uint16_t day = get_num_days(yOff, m, d);
  return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

uint32_t DateTime::unixtime(void) const {
  uint32_t t;
  uint16_t days = get_num_days(yOff, m, d);
  t = get_num_secs(days, hh, mm, ss);
  t += seconds_from_1970_to_2000;  // seconds from 1970 to 2000

  return t;
}


DateTime DateTime::operator + (const TimeSpan& span) {
  return DateTime(unixtime() + span.totalseconds());
}

DateTime DateTime::operator - (const TimeSpan& span) {
  return DateTime(unixtime() - span.totalseconds());
}

TimeSpan DateTime::operator - (const DateTime& right) {
  return TimeSpan(unixtime() - right.unixtime());
}


TimeSpan::TimeSpan(int32_t seconds):
  _seconds(seconds)
{}

TimeSpan::TimeSpan(
  int16_t days,
  int8_t hours,
  int8_t minutes,
  int8_t seconds):
  _seconds(
    (int32_t)days * 86400L +
    (int32_t)hours * 3600 +
    (int32_t)minutes * 60 +
    seconds
  )
{}

TimeSpan::TimeSpan (const TimeSpan& copy):
  _seconds(copy._seconds)
{}

TimeSpan TimeSpan::operator + (const TimeSpan& right) {
  return TimeSpan(_seconds + right._seconds);
}

TimeSpan TimeSpan::operator - (const TimeSpan& right) {
  return TimeSpan(_seconds - right._seconds);
}
