#pragma once

#include <Arduino.h>
#include <me_QueueMind.h>

struct DurationsRec
{
  uint32_t Pause;
  uint32_t Signal;
};

class me_DigitalSignalRecorder
{
  public:
    me_QueueMind Queue;
    DurationsRec* History;
    uint8_t IdleValue = HIGH;

    me_DigitalSignalRecorder(uint8_t aIdleValue = HIGH);
    ~me_DigitalSignalRecorder();
    bool SetCapacity(uint16_t aCapacity);

    void Add(uint32_t CurrentTime, uint8_t Value);
    bool HasEvents();
    uint16_t GetNumElements();
    bool IsFull();
    void Clear();
    uint32_t GetFirstEventTime();
    uint32_t GetLastEventTime();
    uint32_t GetRecordsDuration();

  private:
    uint8_t LastValue = IdleValue;
    volatile uint32_t LastEventTime = 0;
};

void DSR_PrintJSON(me_DigitalSignalRecorder* DSR);
