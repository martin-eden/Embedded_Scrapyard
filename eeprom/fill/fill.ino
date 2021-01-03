#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  fillEeprom();
  Serial.println("Filled.");
}

void loop() {
}

void fillEeprom() {
  for (uint16_t i = 0; i < EEPROM.length(); i++)
    EEPROM[i] = 0xFF;
}