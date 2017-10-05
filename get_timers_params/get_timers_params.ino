// Print values for timer (0, 1, 2) registers.

/*
  Status: done
  Last mod.: 2017-09-10
*/

void setup()
{
  Serial.begin(9600);


  Serial.print("-- timer 0");
  Serial.print("\n");

  Serial.print("TCCR0A: ");
  Serial.print(TCCR0A, BIN);
  Serial.print("\n");

  Serial.print("TCCR0B: ");
  Serial.print(TCCR0B, BIN);
  Serial.print("\n");

  Serial.print("TIMSK0: ");
  Serial.print(TIMSK0, BIN);
  Serial.print("\n");


  Serial.print("-- timer 1");
  Serial.print("\n");

  Serial.print("TCCR1A: ");
  Serial.print(TCCR1A, BIN);
  Serial.print("\n");

  Serial.print("TCCR1B: ");
  Serial.print(TCCR1B, BIN);
  Serial.print("\n");

  Serial.print("TCCR1C: ");
  Serial.print(TCCR1C, BIN);
  Serial.print("\n");

  Serial.print("TIMSK1: ");
  Serial.print(TIMSK1, BIN);
  Serial.print("\n");


  Serial.print("-- timer 2");
  Serial.print("\n");

  Serial.print("TCCR2A: ");
  Serial.print(TCCR2A, BIN);
  Serial.print("\n");

  Serial.print("TCCR2B: ");
  Serial.print(TCCR2B, BIN);
  Serial.print("\n");

  Serial.print("TIMSK2: ");
  Serial.print(TIMSK2, BIN);
  Serial.print("\n");

}

void loop()
{
}
