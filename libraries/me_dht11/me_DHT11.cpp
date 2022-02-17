#include "me_DHT11.h"

const uint8_t
  Status_Init = 0,
  Status_WaitTimeout = 1,
  Status_PacketReceived = 2,
  Status_PacketVerified = 3,
  Status_BadChecksum = 4,
  Status_UncertainBit = 5;

me_DHT11::me_DHT11(uint8_t aDataPin)
{
  DataPin = aDataPin;
}

bool me_DHT11::Get()
{
  Humidity = -1.0;
  Temperature = -1.0;

  if (!ReadData()) return false;
  if (!Verify()) return false;
  if (!Parse()) return false;

  return true;
}

bool me_DHT11::ReadData()
{
  uint8_t DataBits[40] = {1};

  pinMode(DataPin, OUTPUT);
  digitalWrite(DataPin, LOW);
  delay(23); // 11..34
  digitalWrite(DataPin, HIGH);
  delayMicroseconds(40); // 0..83
  pinMode(DataPin, INPUT_PULLUP);

  if (!WaitForLevel(LOW, 80)) return false;
  if (!WaitForLevel(HIGH, 150)) return false;
  if (!WaitForLevel(LOW, 150)) return false;

  for (uint8_t i = 0; i < 40; ++i)
  {
    if (!WaitForLevel(HIGH, 100)) return false;

    uint32_t McrsPassed = WaitForLevel(LOW, 150);
    if (McrsPassed == 0) return false;

    // Bit 0 has duration near 25 us, bit 1 near 75 us.
    if ((McrsPassed > 0) && (McrsPassed <= 35))
      DataBits[i] = 0;
    else if ((McrsPassed >= 40) && (McrsPassed <= 90))
      DataBits[i] = 1;
    else
    {
      Status = Status_UncertainBit;
      return false;
    }
  }

  if (!WaitForLevel(HIGH, 100)) return false;

  for (uint8_t i = 0; i < 5; ++i)
    Data[i] = BitsToByte(DataBits + (i * 8));

  Status = Status_PacketReceived;

  return true;
}

uint32_t me_DHT11::WaitForLevel(uint8_t AwaitedLevel, uint8_t LevelTimeout)
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
  while ((CurrentLevel != AwaitedLevel) && !IsTimeout);

  if (IsTimeout)
  {
    Status = Status_WaitTimeout;
    return 0;
  }

  return TimePassedMcr;
}

uint8_t me_DHT11::BitsToByte(uint8_t Bits[8])
{
  uint8_t Result = 0;
  for (uint8_t i = 0; i < 8; i++)
    Result |= Bits[i] << (7 - i);
  return Result;
}

bool me_DHT11::Verify()
{
  if ((uint8_t) (Data[0] + Data[1] + Data[2] + Data[3]) == Data[4])
    Status = Status_PacketVerified;
  else
    Status = Status_BadChecksum;

  return (Status == Status_PacketVerified);
}

bool me_DHT11::Parse()
{
  Humidity = (float) ((Data[0] << 8) | Data[1]) / 0x100;
  // 2nd temperature byte: tenths of temperature in low nibble in BCD.
  Temperature = (float) ((int16_t) Data[2] * 10 + (Data[3] & 0x0F)) / 10;
  // 2nd temperature byte: bit 7 is temperature sign.
  if (Data[3] & 0x80)
    Temperature = -Temperature;

  return true;
}
