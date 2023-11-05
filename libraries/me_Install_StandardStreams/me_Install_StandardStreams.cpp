// Link STDIN, STDOUT, STDERR to Serial.

/*
  Status: stanble
  Version: 1
  Last mod.: 2023-11-05
*/

/*
  Core part based on StackOverflow answer

    https://forum.arduino.cc/t/printf-on-arduino/888528/5
*/

#include "me_Install_StandardStreams.h"

// ---

int16_t WriteChar(char c, FILE * f);
int16_t ReadChar(FILE * f);

void Install_StandardStreams()
{
  FILE * CustomStream = fdevopen(&WriteChar, &ReadChar);

  /*
    Actually fdevopen() sets std streams to the first stream created by
    its call. So the lines below are redundant. But I want to have more
    reliability.
  */
  stdin = CustomStream;
  stdout = CustomStream;
  stderr = CustomStream;
}

// ---

int16_t WriteChar(char c, FILE * f __attribute__((unused)))
{
  return Serial.write(c);
}

int16_t ReadChar(FILE * f __attribute__((unused)))
{
  return Serial.read();
}

// ---
