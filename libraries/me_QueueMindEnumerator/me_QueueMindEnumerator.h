#pragma once

#include <Arduino.h>
#include <me_QueueSubconscious.h>

class me_QueueMindEnumerator
{
  public:
    me_QueueMindEnumerator(me_QueueSubconscious* aQueue);
    void Reset();
    bool Move();
    uint16_t Get();

  private:
    uint16_t Idx;
    me_QueueSubconscious* Queue;
};
