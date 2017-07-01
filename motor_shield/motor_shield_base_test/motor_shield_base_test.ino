//Test the L298N motor driver shield – Arduino UNO

void setup()
{
  pinMode(2, OUTPUT); //motor B direction 2
  pinMode(3, OUTPUT); //motor B power
  pinMode(4, OUTPUT); //motor B direction 1

  pinMode(5, OUTPUT); //motor A direction 2
  pinMode(6, OUTPUT); //motor A power
  pinMode(7, OUTPUT); //motor A direction 1

  delay(1500);

  digitalWrite(7, HIGH); // Turn on dir 1, Motor A
  digitalWrite(2, HIGH); // Turn on dir 2, Motor B
    analogWrite(6, 100);
    analogWrite(3, 100);
    delay(3000);
    analogWrite(3, 0);
    analogWrite(6, 0);
  digitalWrite(2, LOW);
  digitalWrite(7, LOW);

  delay(1500);

  digitalWrite(5, HIGH); // Turn on dir 2, Motor A
  digitalWrite(4, HIGH); // Turn on dir 1, Motor B
    analogWrite(6, 100);
    analogWrite(3, 100);
    delay(3000);
    analogWrite(3, 0);
    analogWrite(6, 0);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);

}

void loop()
{
}
