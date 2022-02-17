#pragma once

#include <Arduino.h>

#include <me_DHT11.h>

class me_DHT22 : public me_DHT11
{
  public:
    me_DHT22(uint8_t aDataPin) : me_DHT11(aDataPin) {};

  protected:
    virtual bool Parse();
};
