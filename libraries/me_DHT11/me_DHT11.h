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

  protected:
    virtual bool Parse();
    void EmitRequest();

  private:
    uint8_t DataPin;
    bool ReadData();
    uint32_t GetLevelTime(uint8_t OriginalLevel, uint8_t LevelTimeout);
    bool WaitWhileLevel(uint8_t OriginalLevel, uint8_t LevelTimeout);
    uint8_t BitsToByte(uint8_t Bits[8]);
    bool Verify();
};
