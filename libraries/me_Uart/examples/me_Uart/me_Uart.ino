/*
  Board: Arduino Uno
*/

// Last mod.: 2024-03-12

#include <me_Types.h>
#include <me_WS2812B.h>
#include <me_Install_StandardStreams.h>

const TUint_4 SerialSpeed = 57600;

void setup()
{
  Serial.begin(SerialSpeed);
  Install_StandardStreams();
  printf("Hello there!\n");
}

void loop()
{
  delay(1000);
  printf("Tic.\n");
  delay(1000);
  printf("Tac.\n");
}

/*
  2024-03-12
*/
