#pragma once

#include <Arduino.h>

class me_DequeMind
{
  public:
    me_DequeMind(uint16_t aCapacity = 0);
    void Clear();
    uint16_t GetFirstIdx();
    uint16_t GetLastIdx();
    uint16_t GetNumElements();
    uint16_t GetCapacity();
    void SetCapacity(uint16_t aCapacity);
    uint16_t GetNextIdx(uint16_t aIdx);
    uint16_t GetPrevIdx(uint16_t aIdx);
    bool IsFull();
    bool IsEmpty();
    bool AddBefore();
    bool AddAfter();
    bool RemoveFirst();
    bool RemoveLast();

  private:
    uint16_t Capacity;
    uint16_t FirstIdx;
    uint16_t LastIdx;
};

void DequeMind_PrintState(me_DequeMind* Queue);
