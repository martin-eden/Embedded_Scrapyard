#pragma once

#include <Arduino.h>
#include <me_QueueSubconscious.h>

class me_QueueMind : public me_QueueSubconscious
{
  public:
    bool Enqueue();
    bool Dequeue();
};
