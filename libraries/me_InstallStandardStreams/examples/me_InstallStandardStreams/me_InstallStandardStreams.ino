// Demo of [me_InstallStandardStreams]

/*
  InstallStandardStreams();
  ..
  printf("Yeah baby!\n");
*/

/*
  Author: Martin Eden
  Last mod.: 2024-05-09
*/

#include <me_InstallStandardStreams.h>
#include <me_Types.h>

void setup()
{
  Serial.begin(57600);

  InstallStandardStreams();

  printf("[me_InstallStandardStreams.ino] Hello there!\n");
}

void loop()
{
  TUint_4 SleepTime_ms = (5 + random(3)) * 1000;
  delay(SleepTime_ms);

  TUint_4 SecondsPassed = millis() / 1000;

  printf("%lu seconds passed\n", SecondsPassed);
}
