/*
  JeeLab, LadyAda - some code based on these. But I felt that code was
  too ugly to keep and tried to rework and simplify it.

  2019-02-24, Martin
*/

#include "me_ds3231.h"
#include <Wire.h>

uint8_t bcdToDec(uint8_t val)
{
  return (val / 16 * 10) + (val % 16);
}

uint8_t decToBcd(uint8_t val)
{
  return (val / 10 * 16) + (val % 10);
}

const uint8_t DS3231_ID = 0x68;

const uint8_t
  CONTROL = 0x0E,
  STATUS = 0x0F,
  AGING = 0x10,
  TEMP_INT = 0x11,
  TEMP_FRAC = 0x12;

uint8_t getByte(uint8_t offs)
{
  Wire.beginTransmission(DS3231_ID);
  Wire.write(offs);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_ID, (uint8_t) 1);

  return Wire.read();
}

void setByte(uint8_t offs, uint8_t val)
{
  Wire.beginTransmission(DS3231_ID);
  Wire.write(offs);
  Wire.write(val);
  Wire.endTransmission();
}

void requestBytes(uint8_t offs, uint8_t numBytes, uint8_t* buf)
{
  Wire.beginTransmission(DS3231_ID);
  Wire.write(offs);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_ID, numBytes);
  for (uint8_t i = 0; i < numBytes; i++)
    buf[i] = Wire.read();
}

void writeBytes(uint8_t offs, uint8_t numBytes, uint8_t* buf)
{
  Wire.beginTransmission(DS3231_ID);
  Wire.write(offs);
  Wire.write(buf, numBytes);
  Wire.endTransmission();
}

me_ds3231::me_ds3231()
{
  Wire.begin();
}

uint8_t me_ds3231::getDoW()
{
  return getByte(3);
}

void me_ds3231::setDoW(uint8_t DoW)
{
  setByte(3, DoW);
}

DateTime me_ds3231::getDateTime()
{
  uint8_t buf[7];
  requestBytes(0, 7, buf);

  uint8_t ss = bcdToDec(buf[0] & 0x7F);
  uint8_t mm = bcdToDec(buf[1]);
  uint8_t hh = bcdToDec(buf[2]);
  uint8_t d = bcdToDec(buf[4]);
  uint8_t m = bcdToDec(buf[5]);
  uint16_t y = bcdToDec(buf[6]) + 2000;

  return DateTime(y, m, d, hh, mm, ss);
}

void me_ds3231::setDateTime(DateTime dt)
{
  uint8_t buf[7];

  buf[0] = decToBcd(dt.second());
  buf[1] = decToBcd(dt.minute());
  buf[2] = decToBcd(dt.hour());
  buf[3] = getDoW();
  buf[4] = decToBcd(dt.day());
  buf[5] = decToBcd(dt.month());
  buf[6] = decToBcd((dt.year() - 2000));

  writeBytes(0, 7, buf);
}

float me_ds3231::getTemp()
/*
  Read measured temperature. Quick. (Temperature is auto-measured
  once in 64 seconds.)

  return reg[0x11] + (reg[0x12] >> 6) / 4
*/
{
  float result =
    int8_t(getByte(TEMP_INT)) +
    (float(getByte(TEMP_FRAC) >> 6) / 4);

  return result;
}

float me_ds3231::measureTemp()
/*
  Measure temperature and return result. Takes some time (near .3s).

  while flag.busy;
  flag.conv = 1;
  while flag.conv;
  return get_temp()
*/
{
  // naming used bits of registers:
  const uint8_t
    CONTROL_CONV = 5,
    STATUS_BUSY = 2;

  while (getByte(STATUS) & _BV(STATUS_BUSY));

  uint8_t control_reg;
  control_reg = getByte(CONTROL);
  control_reg |= _BV(CONTROL_CONV);
  setByte(CONTROL, control_reg);

  while (getByte(CONTROL) & _BV(CONTROL_CONV));

  return getTemp();
}

/*
  mode | RS2,RS1 | freq (Hz)
  -----+---------+----------
    0  |    00   | 1
    1  |    01   | 1024
    2  |    10   | 4096
    3  |    11   | 8192
*/
const uint8_t
  RS1 = 3,
  RS2 = 4;

void me_ds3231::setSqwMode(uint8_t mode)
{
  uint8_t control_reg = getByte(CONTROL);
  switch(mode)
  {
    case 0:
      control_reg &= ~_BV(RS2);
      control_reg &= ~_BV(RS1);
      break;
    case 1:
      control_reg &= ~_BV(RS2);
      control_reg |= _BV(RS1);
      break;
    case 2:
      control_reg |= _BV(RS2);
      control_reg &= ~_BV(RS1);
      break;
    case 3:
      control_reg |= _BV(RS2);
      control_reg |= _BV(RS1);
      break;
  }
  setByte(CONTROL, control_reg);
}

uint8_t me_ds3231::getSqwMode()
{
  uint8_t control_reg = getByte(CONTROL);
  uint8_t result =
    (control_reg & (_BV(RS1) | _BV(RS2))) >> RS1;
  return result;
}


const uint8_t INTCN = 2;

bool me_ds3231::isSqw()
{
  uint8_t control_reg = getByte(CONTROL);
  bool result = !(control_reg & _BV(INTCN));
  return result;
}

void me_ds3231::enableSqw()
{
  uint8_t control_reg = getByte(CONTROL);
  control_reg &= ~_BV(INTCN);
  setByte(CONTROL, control_reg);
}

void me_ds3231::disableSqw()
{
  uint8_t control_reg = getByte(CONTROL);
  control_reg |= _BV(INTCN);
  setByte(CONTROL, control_reg);
}


const uint8_t BBSQW = 6;

bool me_ds3231::isSqwAtBattery()
{
  uint8_t control_reg = getByte(CONTROL);
  bool result = (control_reg & _BV(BBSQW));
  return result;
}

void me_ds3231::enableSqwAtBattery()
{
  uint8_t control_reg = getByte(CONTROL);
  control_reg |= _BV(BBSQW);
  setByte(CONTROL, control_reg);
}

void me_ds3231::disableSqwAtBattery()
{
  uint8_t control_reg = getByte(CONTROL);
  control_reg &= ~_BV(BBSQW);
  setByte(CONTROL, control_reg);
}


const uint8_t EOSC = 7;

bool me_ds3231::isOscillatorAtBattery()
{
  uint8_t control_reg = getByte(CONTROL);
  bool result = !(control_reg & _BV(EOSC));
  return result;
}

void me_ds3231::enableOscillatorAtBattery()
{
  uint8_t control_reg = getByte(CONTROL);
  control_reg &= ~_BV(EOSC);
  setByte(CONTROL, control_reg);
}

void me_ds3231::disableOscillatorAtBattery()
{
  uint8_t control_reg = getByte(CONTROL);
  control_reg |= _BV(EOSC);
  setByte(CONTROL, control_reg);
}


const uint8_t OSF = 7;

bool me_ds3231::oscillatorWasStopped()
{
  uint8_t status_reg = getByte(STATUS);
  bool result = (status_reg & _BV(OSF));
  return result;
}

void me_ds3231::clearOscillatorWasStopped()
{
  uint8_t status_reg = getByte(STATUS);
  status_reg &= ~_BV(OSF);
  setByte(STATUS, status_reg);
}
