// Prototype for data running over data space of EEPROM.

/*
  At offset 0 position of record is stored.

  This is drawback as at every record position update value at
  offset 0 is updated.
*/

#include <EEPROM.h>

struct SampleRec {
  uint32_t data1;
  uint32_t data2;
};

SampleRec dataRec = {1, 2};

void initRunningData() {
  uint16_t offset;
  EEPROM.get(0, offset);
  if ((offset == 0xFFFF) or ((offset % 4) != 0)) {
    offset = 2;
    EEPROM.put(offset, dataRec);
    EEPROM.put(0, offset);
  }
}

void updateDataRec(struct SampleRec* aRec) {
  aRec->data1 = aRec->data1 + 1;
  aRec->data2 = aRec->data2 + 2;
}

struct SampleRec getDataRec() {
  uint16_t offset;
  EEPROM.get(0, offset);
  SampleRec result;
  EEPROM.get(offset, result);
  return result;
}

uint16_t updateOffset(uint16_t aOffset) {
  uint16_t result = aOffset;
  result += sizeof(SampleRec);
  if (result >= EEPROM.length()) {
    result = 2; // first two bytes are pointer
  }
  return result;
}

void setDataRec(struct SampleRec aRec) {
  uint16_t offset;
  EEPROM.get(0, offset);
  offset = updateOffset(offset);
  EEPROM.put(offset, aRec);
  EEPROM.put(0, offset);
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

  initRunningData();

  for (uint16_t i = 0; i < 129; ++i) {
    dataRec = getDataRec();
    updateDataRec(&dataRec);
    setDataRec(dataRec);
  }

  dumpEeprom();
}

void loop() {
}