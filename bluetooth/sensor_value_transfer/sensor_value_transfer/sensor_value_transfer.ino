#include <MeetAndroid.h>

MeetAndroid meetAndroid;
int sensor = 11;

void setup()  
{
  Serial.begin(9600); 
 
  pinMode(sensor, INPUT);
}

void loop()
{
  meetAndroid.receive(); // you need to keep this in your loop() to receive events
  
  // read input pin and send result to Android
  meetAndroid.send(analogRead(sensor));
  
  // add a little delay otherwise the phone is pretty busy
  delay(1500);
}


