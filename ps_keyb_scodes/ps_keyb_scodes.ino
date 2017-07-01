#include <PS2Keyboard.h>

const int DataPin = 4;
const int IRQpin =  3;

PS2Keyboard keyboard;

void setup() {
  delay(1000);
  keyboard.begin(DataPin, IRQpin);
  Serial.begin(9600);
  Serial.println("Keyboard Test:");
}

void loop() {
  if (keyboard.available()) {
    
    // read the next key
    uint8_t c = keyboard.read();
    if (c != 255) 
    {
      Serial.println(c, HEX);  
    }    
  }
}

