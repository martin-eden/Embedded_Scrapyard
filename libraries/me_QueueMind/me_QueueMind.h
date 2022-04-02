#pragma once

#include <Arduino.h>

class me_QueueMind
{
  public:
    me_QueueMind(uint16_t aCapacity = 0);
    void Clear();
    uint16_t GetFirstIdx();
    uint16_t GetLastIdx();
    uint16_t GetCount();
    uint16_t GetCapacity();
    void SetCapacity(uint16_t aCapacity);
    uint16_t GetNextIdx(uint16_t aIdx);
    bool IsFull();
    bool IsEmpty();
    bool Enqueue();
    bool Dequeue();

  private:
    uint16_t Capacity;
    uint16_t FirstIdx;
    uint16_t LastIdx;
};

void QueueMind_PrintState(me_QueueMind* Queue);
