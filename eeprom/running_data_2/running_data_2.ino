// Prototype for data running over data space of EEPROM.

/*
  Equal EEPROM memory usage.

  We define "empty" slot as slot containing FF's.

  To add data we searching first empty slot, write data there, move offset to
  next slot and write empty data there.
*/

#include <EEPROM.h>

struct SampleRec {
  uint32_t data1;
  uint32_t data2;
};

SampleRec dataRec = {1, 2};

void fillEeprom() {
  for (uint16_t i = 0; i < EEPROM.length(); i++)
    EEPROM[i] = 0xFF;
}

void initRunningData() {
  fillEeprom();
}

void updateDataRec(struct SampleRec* aRec) {
  aRec->data1 = aRec->data1 + 1;
  aRec->data2 = aRec->data2 + 2;
}

uint16_t decreaseOffset(uint16_t aOffset) {
  uint16_t result = aOffset;
  if (result < sizeof(SampleRec))
    result = EEPROM.length();
  result = result - sizeof(SampleRec);
  return result;
}

struct SampleRec getDataRec() {
  SampleRec result;
  uint16_t offset = findEmptyRec();
  offset = decreaseOffset(offset);
  EEPROM.get(offset, result);
  return result;
}

uint16_t increaseOffset(uint16_t aOffset) {
  uint16_t result = aOffset;
  result += sizeof(SampleRec);
  if (result >= EEPROM.length())
    result = 0;
  return result;
}

bool isEmptyRec(struct SampleRec aRec) {
  return (aRec.data1 == 0xFFFFFFFF) && (aRec.data2 == 0xFFFFFFFF);
};

uint16_t findEmptyRec() {
  uint16_t result = 0xFFFF;
  for(uint16_t offset = 0; offset < EEPROM.length(); offset += sizeof(SampleRec)) {
    SampleRec rec;
    EEPROM.get(offset, rec);
    if (isEmptyRec(rec)) {
      result = offset;
      break;
    }
  }
  if (result == 0xFFFF) {
    fillEeprom();
    result = findEmptyRec();
  }
  return result;
}

void setEmptyRec(uint16_t aOffset) {
  SampleRec rec = {0xFFFFFFFF, 0xFFFFFFFF};
  EEPROM.put(aOffset, rec);
}

void addDataRec(struct SampleRec aRec) {
  uint16_t offset = findEmptyRec();
  EEPROM.put(offset, aRec);
  offset = increaseOffset(offset);
  setEmptyRec(offset);
}

void dumpEeprom() {
  for (uint16_t i = 0; i < EEPROM.length(); i++) {
    Serial.print(EEPROM[i], HEX);
    Serial.print(" ");
    if ((i & 0x1F) == 0x1F)
      Serial.println();
  }
}

void setup() {
  Serial.begin(9600);

  dumpEeprom();

  for (uint16_t i = 0; i < 10; ++i) {
    dataRec = getDataRec();
    updateDataRec(&dataRec);
    addDataRec(dataRec);
  }

  dumpEeprom();
}

void loop() {
}