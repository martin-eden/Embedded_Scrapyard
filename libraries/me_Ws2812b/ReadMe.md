# What

Arduino library to control LED stripe.

My journey to the land of microcontrollers programming and back.
There are some dragons.

# Buzzwords description

Arduino. AVR. ATmega328/P. 16 MHz. RGB LED stripe. WS2812B. Function.
C. Assembler. Standalone implementation. GPL3.

# How

```C++
#include <me_Ws2812b.h>
...
using namespace me_Ws2812b;

TPixel TestPixels[] =
  {
    { .Green = 0xFF, .Red = 0, .Blue = 0 },
    { .Green = 0, .Red = 0xFF, .Blue = 0 },
    { .Green = 0, .Red = 0, .Blue = 0xFF },
  };

SendPixels(
  TestPixels,
  sizeof(TestPixels) / sizeof(TPixel),
  A0
);
```

See [example](examples/me_Ws2812b/me_Ws2812b.ino) for real code.

# Code

Code is readable.

* [Interface](src/me_Ws2812b.h)
* [Implementation](src/me_Ws2812b.cpp)

# Other people implementations

I'm aware of them and intentionally decided to write my own.

Main issue there is bad design. It's unlimited. They are trying to write
multi-platform code in C lol. And they are putting all the fancy
color manipulation routines in their libraries. Neverending projects.

More importantly, you can't change LED output pin at runtime there.

* [FastLED](https://github.com/FastLED/FastLED)
* [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)

# See also

* [Implementation notes](extras/Implementation%20notes.txt)
* [Additional documentation](https://github.com/martin-eden/EmbeddedCpp-me_Ws2812b-Docs)
  (Datasheet, oscilloscope timings, sticky pins weirdness.)
* [My other repositories](https://github.com/martin-eden/contents)

-- Martin, 2024-03 / 2024-05
