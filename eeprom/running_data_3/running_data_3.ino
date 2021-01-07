#include <me_EepromCircularStack.h>

EepromCircularStack stack = EepromCircularStack(sizeof(float));

void setup() {
  Serial.begin(9600);

  while (stack.remove());

  stack.dump();

  /*
  for (uint16_t i = 0; i < 16; ++i) {
    int16_t offset = stack.add_offs();
    float value = 16 * i;
    EEPROM.put(offset, value);
  }

  stack.dump();
  */
}

void loop() {
}