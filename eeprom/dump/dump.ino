#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  dumpEeprom();
}

void loop() {
}

void dumpEeprom() {
  for (uint16_t i = 0; i < EEPROM.length(); i++) {
    Serial.print(EEPROM[i], HEX);
    Serial.print(" ");
    if ((i & 0x1F) == 0x1F)
      Serial.println();
  }
}