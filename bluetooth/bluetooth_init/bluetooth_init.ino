// This sketch configures a bluetooth module connected to pins 0 and 1
// Therefore, the usb cable has to be removed after uploading this program
// and the Arduino card must be powered by external source.

const int LED_PIN = 13;
int randomValue = 0;
// 1000 ms of no data transmission required before and after the escape sequence
int cmdDelay = 1000;
// Change these two paramaters to anything you want
int pin = 1328;
//char* name = "Pcr-1";

void setup()
{
/*  
  pinMode(LED_PIN, OUTPUT);
  // Turn on LED to signal programming start
  digitalWrite(LED_PIN, HIGH);
*/
  Serial.begin(9600);
/*  
  delay(cmdDelay);
  Serial.print("AT");
  delay(cmdDelay);
  Serial.print("AT+PIN");
  Serial.print(pin);
  delay(cmdDelay);
  Serial.print("AT+NAME");
  Serial.print(name);
  delay(cmdDelay);
  // Turn off LED to signal programming end
  digitalWrite(LED_PIN, LOW);
*/  
}

// This part is not doing anything useful
void loop() {
  if (Serial.available())
  {
    Serial.read();
    randomValue = random(0, 9);
    Serial.println(randomValue);
  }
}
