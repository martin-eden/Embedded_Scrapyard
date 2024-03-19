#include <me_Types.h>

void doSomething()
{
  TUint_1 a, b, seed, d;
  seed = analogRead(A0);
  randomSeed(seed);
  a = random();
  a = 123;
  b = random();
  d = a / b;
  Serial.println(seed);
  Serial.println(a);
  Serial.println(b);
  Serial.println(d);
}

void setup()
{
  Serial.begin(57600);

  doSomething();
}

void loop()
{
}
