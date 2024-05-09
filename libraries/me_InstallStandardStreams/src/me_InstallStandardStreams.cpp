// Direct (stdin, stdout, stderr) to Serial

/*
  Wrapped by: Martin Eden
  Last mod.: 2024-05-09
*/

#include "me_InstallStandardStreams.h"

#include <Arduino.h> // fdevopen(), FILE, stdin..

#include <me_Types.h> // TSint_2, TChar

// Forwards:
TSint_2 WriteChar(TChar Char, FILE * File);
TSint_2 ReadChar(FILE * File);

/*
  Assign standard streams stdin, stdout, stderr to Serial.

  Expected to be called once.

  Based on Arduino's forum answer

    https://forum.arduino.cc/t/printf-on-arduino/888528/5
*/
void InstallStandardStreams()
{
  FILE * SerialStream = fdevopen(&WriteChar, &ReadChar);
  /*
    fdevopen() creates stream and has side effects.

    Side effect are setting std streams to newly created stream
    (if they were not set).

    We are calling it for it's side effects but still assign
    stream to std's just in case they were set.

    Documentation found at

      https://onlinedocs.microchip.com/pr/GUID-317042D4-BCCE-4065-BB05-AC4312DBC2C4-en-US-2/index.html?GUID-9EF470A9-BEB2-4014-8FC3-5B39F3F86DAF

      https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gab599ddf60819df4cc993c724a83cb1a4

    (I just searched for "C fdevopen".)
  */

  stdin = SerialStream;
  stdout = SerialStream;
  stderr = SerialStream;
}

/*
  Write char

  Docs:

    The function passed as put shall take two arguments, the first a
    character to write to the device, and the second a pointer to FILE,
    and shall return 0 if the output was successful, and a nonzero
    value if the character could not be sent to the device.
*/
TSint_2 WriteChar(TChar Char, FILE * File __attribute__((unused)))
{
  /*
    Serial.write() always returns 1. According to docs it should
    return 0. But it works and I won't meddle with code now.
  */
  return Serial.write(Char);
}

/*
  Read char

  Docs:

    The function passed as get shall take a pointer to FILE as its
    single argument, and return one character from the device, passed
    as an int type.

    If an error occurs when trying to read from the device, it shall
    return _FDEV_ERR. If an end-of-file condition was reached while
    reading from the device, _FDEV_EOF shall be returned.
*/
TSint_2 ReadChar(FILE * File __attribute__((unused)))
{
  /*
    Serial.read() returns -1 when stream is empty.
    (_FDEV_ERR == -1), (_FDEV_EOF == -2). So it should work.

    I hadn't test stdin functionality. This function is for completeness
    but implementation may be changed if it does not work.
  */
  return Serial.read();
}

/*
  2023-11-05
  2024-05-09
*/
