#include <Arduino.h>

#include "DateTime.h"

class me_ds3231
{
  public:
    me_ds3231();

    uint8_t getDoW();
    void setDoW(uint8_t DoW);

    DateTime getDateTime();
    void setDateTime(DateTime);

    float getTemp();
    float measureTemp();

    void setSqwMode(uint8_t mode);
    uint8_t getSqwMode();

    bool isSqw();
    void emitSqwNoAlarm();
    void emitAlarmNoSqw();

    bool isSqwAtBattery();
    void enableSqwAtBattery();
    void disableSqwAtBattery();

    bool isOscillatorAtBattery();
    void enableOscillatorAtBattery();
    void disableOscillatorAtBattery();

    bool oscillatorWasStopped();
    void clearOscillatorWasStopped();

    bool wave32kEnabled();
    void enable32kWave();
    void disable32kWave();
};
