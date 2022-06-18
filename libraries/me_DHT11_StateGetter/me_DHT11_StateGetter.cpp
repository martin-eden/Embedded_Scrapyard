#include "me_DHT11_StateGetter.h"
#include <me_DHT11.h>

using namespace me_DHT11_StateGetter;

State me_DHT11_StateGetter::GetState(me_DHT11::DHT11* sensor)
{
  State result;

  if (sensor->LastReadStatus == me_DHT11::ReadStatus::Success)
  {
    result.IsValid = true;
    result.Temperature = sensor->Temperature;
    result.Humidity = sensor->Humidity;
  }
  else
  {
    result.IsValid = false;
    result.Temperature = 0.0;
    result.Humidity = 0.0;
  }

  return result;
}
