#include "me_DHT22.h"

bool me_DHT22::Parse()
{
  Humidity = (float) ((int16_t) (Data[0] << 8) | Data[1]) / 10;
  Temperature = (float) ((int16_t) ((Data[2] & 0x7F) << 8) | Data[3]) / 10;
  if (Data[2] & 0x80)
    Temperature = -Temperature;

  return true;
}
