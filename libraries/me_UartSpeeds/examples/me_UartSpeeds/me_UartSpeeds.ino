// [me_UartSpeeds] usage illustration

// Last mod.: 2024-05-05

#include <me_UartSpeeds.h>

void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  Serial.println("[me_UartSpeeds.ino] Hello there!");
}

void loop()
{
}

/*
  2024-03-12
  2024-05-05
*/
