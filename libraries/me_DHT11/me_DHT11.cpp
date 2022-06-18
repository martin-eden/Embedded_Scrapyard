#include "me_DHT11.h"

using namespace me_DHT11;

const uint8_t
  Status_Init = 0,
  Status_WaitTimeout = 1,
  Status_PacketReceived = 2,
  Status_PacketVerified = 3,
  Status_BadChecksum = 4,
  Status_UncertainBit = 5;

DHT11::DHT11(uint8_t aDataPin)
{
  DataPin = aDataPin;
}

void DHT11::Request()
{
  LastReadStatus = ReadStatus::Error;
  Humidity = -1.0;
  Temperature = -1.0;

  if (!ReadData()) return;
  if (!Verify()) return;
  if (!Parse()) return;

  LastReadStatus = ReadStatus::Success;
}

bool DHT11::ReadData()
{
  uint8_t DataBits[40] = {1};

  EmitRequest();

  if (!WaitWhileLevel(HIGH, 80)) return false;
  if (!WaitWhileLevel(LOW, 150)) return false;
  if (!WaitWhileLevel(HIGH, 150)) return false;

  for (uint8_t i = 0; i < 40; ++i)
  {
    if (!WaitWhileLevel(LOW, 100)) return false;

    uint32_t PulseDuration = GetLevelTime(HIGH, 150);
    if (PulseDuration == 0) return false;

    // Bit 0 has duration near 25 us, bit 1 near 75 us.
    if ((PulseDuration > 0) && (PulseDuration <= 35))
      DataBits[i] = 0;
    else if ((PulseDuration >= 40) && (PulseDuration <= 90))
      DataBits[i] = 1;
    else
    {
      Status = Status_UncertainBit;
      return false;
    }
  }

  if (!WaitWhileLevel(LOW, 100)) return false;

  for (uint8_t i = 0; i < 5; ++i)
    Data[i] = BitsToByte(DataBits + (i * 8));

  Status = Status_PacketReceived;

  return true;
}

void DHT11::EmitRequest()
{
  pinMode(DataPin, OUTPUT);
  digitalWrite(DataPin, LOW);
  delay(23); // 11..34
  digitalWrite(DataPin, HIGH);
  delayMicroseconds(40); // 0..83
  pinMode(DataPin, INPUT_PULLUP);
}

uint32_t DHT11::GetLevelTime(uint8_t OriginalLevel, uint8_t LevelTimeout)
{
  uint8_t CurrentLevel;
  uint32_t TimePassedMcr;
  uint32_t StartTime = micros();
  bool IsTimeout;
  do
  {
    CurrentLevel = digitalRead(DataPin);
    TimePassedMcr = (micros() - StartTime);
    IsTimeout = (TimePassedMcr > LevelTimeout);
  }
  while ((CurrentLevel == OriginalLevel) && !IsTimeout);

  if (IsTimeout)
  {
    Status = Status_WaitTimeout;
    return 0;
  }

  return TimePassedMcr;
}

bool DHT11::WaitWhileLevel(uint8_t OriginalLevel, uint8_t LevelTimeout)
{
  return (GetLevelTime(OriginalLevel, LevelTimeout) != 0);
}

uint8_t DHT11::BitsToByte(uint8_t Bits[8])
{
  uint8_t Result = 0;
  for (uint8_t i = 0; i < 8; i++)
    Result |= Bits[i] << (7 - i);
  return Result;
}

bool DHT11::Verify()
{
  if ((uint8_t) (Data[0] + Data[1] + Data[2] + Data[3]) == Data[4])
    Status = Status_PacketVerified;
  else
    Status = Status_BadChecksum;

  return (Status == Status_PacketVerified);
}

bool DHT11::Parse()
{
  Humidity = (float) ((Data[0] << 8) | Data[1]) / 0x100;
  // 2nd temperature byte: tenths of temperature in low nibble in BCD.
  Temperature = (float) ((int16_t) Data[2] * 10 + (Data[3] & 0x0F)) / 10;
  // 2nd temperature byte: bit 7 is temperature sign.
  if (Data[3] & 0x80)
    Temperature = -Temperature;

  return true;
}

/*
  Implementation notice.

  I've tried implementation via standard Arduino library function
  pulseInLong() instead of WaitForLevel(). It is possible and makes
  code shorter. But the drawback is that we can no longer track LOW
  signal times. So implementation will be more loose to protocol.
*/
