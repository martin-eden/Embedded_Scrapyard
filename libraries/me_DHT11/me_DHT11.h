#pragma once

#include <Arduino.h>

namespace me_DHT11
{
  enum class ReadStatus
  {
    Unknown,
    Success,
    Error
  };

  class DHT11
  {
    public:
      float Humidity;
      float Temperature;
      uint8_t Status;
      ReadStatus LastReadStatus;
      uint8_t Data[5];

      DHT11(uint8_t aDataPin);

      void Request();

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
}
