#pragma once

#include <Arduino.h>

class me_QueueSubconscious
{
  public:
    me_QueueSubconscious(uint16_t aCapacity = 0);
    bool IsEmpty();
    void MakeEmpty();
    uint16_t GetFirstIdx();
    uint16_t GetLastIdx();
    uint16_t GetCount();
    uint16_t GetCapacity();
    void SetCapacity(uint16_t aCapacity);
    bool IsFull();
    uint16_t GetNextIdx(uint16_t aIdx);

  protected:
    uint16_t FirstIdx;
    uint16_t LastIdx;
    bool _IsEmpty;

  private:
    uint16_t Capacity;
};
