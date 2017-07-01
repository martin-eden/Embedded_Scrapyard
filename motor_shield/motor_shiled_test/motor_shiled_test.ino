//Test the L298N motor driver shield – Arduino UNO

void setup()
{
  pinMode(2, OUTPUT); //motor B direction 2
  pinMode(3, OUTPUT); //motor B power
  pinMode(4, OUTPUT); //motor B direction 1

  pinMode(5, OUTPUT); //motor A direction 2
  pinMode(6, OUTPUT); //motor A power
  pinMode(7, OUTPUT); //motor A direction 1

  Serial.begin(9600);
}

void loop()
{
  char c = Serial.read();

  switch(c)
  {
    case 'L':
      digitalWrite(5, LOW);
      digitalWrite(7, HIGH);
      break;
    case 'l':
      digitalWrite(7, LOW);
      digitalWrite(5, HIGH);
      break;
    case 'A':
      analogWrite(6, 42);
      break;
    case 'B':
      analogWrite(6, 84);
      break;
    case 'C':
      analogWrite(6, 127);
      break;
    case 'D':
      analogWrite(6, 170);
      break;
    case 'E':
      analogWrite(6, 212);
      break;
    case 'F':
      analogWrite(6, 255);
      break;
    case '0':
      analogWrite(6, 0);
      digitalWrite(7, LOW);
      digitalWrite(5, LOW);
      break;
    case 'R':
      digitalWrite(2, LOW);
      digitalWrite(4, HIGH);
      break;
    case 'r':
      digitalWrite(4, LOW);
      digitalWrite(2, HIGH);
      break;
    case 'a':
      analogWrite(3, 42);
      break;
    case 'b':
      analogWrite(3, 84);
      break;
    case 'c':
      analogWrite(3, 127);
      break;
    case 'd':
      analogWrite(3, 170);
      break;
    case 'e':
      analogWrite(3, 212);
      break;
    case 'f':
      analogWrite(3, 255);
      break;
    case 'O':
      analogWrite(3, 0);
      digitalWrite(4, LOW);
      digitalWrite(2, LOW);
      break;
  }
}
