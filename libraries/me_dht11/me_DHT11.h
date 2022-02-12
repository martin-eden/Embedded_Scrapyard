#pragma once

#include <Arduino.h>

#define me_DHT11_CompileError(t, err) (((t << 8) & 0xff00) | (err & 0x00ff))

const uint8_t
  me_DHT11_OK = 0,
  me_DHT11_ErrStartLow = 0x10,
  me_DHT11_ErrStartHigh = 0x11,
  me_DHT11_ErrDataLow = 0x12,
  me_DHT11_ErrDataRead = 0x13,
  me_DHT11_ErrDataEOF = 0x14,
  me_DHT11_ErrDataChecksum = 0x15,
  me_DHT11_ErrZeroSamples = 0x16,
  me_DHT11_ErrNoPin = 0x17;

class me_DHT11
{
  public:
    me_DHT11(uint8_t aDataPin);

    uint8_t Read(float* HumidityPtr, float* TemperaturePtr);
    uint8_t ReadRaw(uint16_t* HumidityPtr, uint16_t* TemperaturePtr);
  private:
    uint32_t LevelTimeout = 100000;
    uint8_t DataPin = 0;
    uint8_t PinBitmask = 0xFF;
    uint8_t PinPort = 0xFF;

    void SetDataPin(uint8_t aDataPin);
    uint8_t Sample(uint8_t Data[40]);
    uint32_t LevelTime(uint8_t Level, uint8_t FirstWait = 10, uint8_t Interval = 6);
    uint8_t Convert(uint8_t Data[40], uint16_t* HumidityPtr, uint16_t* TemperaturePtr);
    uint8_t BitsToByte(uint8_t Bits[8]);
};
