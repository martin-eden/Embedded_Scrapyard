// Circular stack in EEPROM.

/*
  Idea is not to use it as stack, but just store one record. That record
  position will be shifted after every addition resulting in equal EEPROM
  memory usage.

  So in this implementation stack can overrun on ifself without warning.

  To add data we searching first empty slot, write data there, and write
  empty data in next slot.
*/

#include <EEPROM.h>

class EepromCircularStack {
  public:
    EepromCircularStack(uint16_t a_slotSize);
    int16_t get_offs();
    uint16_t add_offs();
    bool remove();
    void dump();
  private:
    uint16_t slotSize;
    bool isEmptySlot(uint16_t offset);
    void setEmptySlot(uint16_t offset);
    int16_t findFirstEmptySlot();
    uint16_t increaseOffset(uint16_t offset);
    uint16_t decreaseOffset(uint16_t offset);
};

EepromCircularStack::EepromCircularStack(uint16_t a_slotSize) {
  slotSize = a_slotSize;
}

bool EepromCircularStack::isEmptySlot(uint16_t offset) {
  for (uint16_t i = 0; i < slotSize; ++i) {
    if (EEPROM.read(i + offset) != 0xFF)
      return false;
  }
  return true;
}

void EepromCircularStack::setEmptySlot(uint16_t offset) {
  if (offset + slotSize > EEPROM.length())
    return;
  for (uint16_t i = 0; i < slotSize; ++i)
    EEPROM.update(i + offset, 0xFF);
}

uint16_t EepromCircularStack::increaseOffset(uint16_t offset) {
  uint16_t result = offset + slotSize;
  if (result >= EEPROM.length())
    result = 0;
  return result;
}

uint16_t EepromCircularStack::decreaseOffset(uint16_t offset) {
  uint16_t result = offset;
  if (result < slotSize)
    result = EEPROM.length();
  result -= slotSize;
  return result;
}

int16_t EepromCircularStack::findFirstEmptySlot() {
  int16_t result = -1;
  uint16_t offset = 0;
  while (true) {
    if (isEmptySlot(offset)) {
      result = offset;
      break;
    }
    offset = increaseOffset(offset);
    if (offset == 0)
      break;
  }
  return result;
}

int16_t EepromCircularStack::get_offs() {
  int16_t result = findFirstEmptySlot();
  if (result != -1)
    result = decreaseOffset(result);
  if (isEmptySlot(result))
    result = -2;
  return result;
}

uint16_t EepromCircularStack::add_offs() {
  int16_t result = findFirstEmptySlot();
  if (result == -1)
    result = 0;
  setEmptySlot(increaseOffset(result));
  return result;
}

bool EepromCircularStack::remove() {
  bool result = true;
  int16_t offset = get_offs();
  if (offset == -2)
    result = false;
  else {
    if (offset == -1)
      offset = 0;
    setEmptySlot(offset);
  }
  return result;
}

void EepromCircularStack::dump() {
  Serial.println();
  for (uint16_t i = 0; i < EEPROM.length(); i++) {
    uint8_t value = EEPROM.read(i);
    Serial.print(value >> 4, HEX);
    Serial.print(value & 0xF, HEX);
    Serial.print(" ");
    if ((i & 0x1F) == 0x1F)
      Serial.println();
  }
  Serial.println();
}
