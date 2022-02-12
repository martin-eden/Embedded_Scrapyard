#pragma once

#include <Arduino.h>

class me_DHT11
{
  public:
    float Humidity;
    float Temperature;
    uint8_t Status;
    uint8_t Data[5];

    me_DHT11(uint8_t aDataPin);

    bool Get();
  private:
    uint8_t DataPin;
    void ReadData();
    uint32_t WaitForLevel(uint8_t AwaitedLevel, uint8_t LevelTimeout);
    uint8_t BitsToByte(uint8_t Bits[8]);
    void Verify();
    void Parse();
};
