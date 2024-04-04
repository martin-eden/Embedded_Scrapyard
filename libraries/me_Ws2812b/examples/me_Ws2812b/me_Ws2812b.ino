// Test of [me_Ws2821b] library.

/*
  This library provides two functions:

    * SendPacket(Bytes[], NumBytes, OutputPin): bool

    * SendPixels(Pixels[], NumPixels, OutputPin): bool

      Pixel is a color from three components: { .Green, .Red, .Blue }

  Keep in mind that each pixel of WS2821B LED stripe keeps last written
  value.
*/

/*
  Board: Arduino Uno

  Author: Martin Eden
  Development: 2024-02/2024-04
  Last mod.: 2024-04-04
*/

#include <me_Types.h>
#include <me_Uart.h>
#include <me_Install_StandardStreams.h>
#include <me_Math.h>

#include <me_Ws2812b.h>

// --

const TUint_1 LedStripePin = A1;

const TUint_1 NumPixels = 60;

// --

void setup()
{
  const TUint_4 SerialSpeed = me_Uart::Arduino_Normal_Bps;
  Serial.begin(SerialSpeed);
  delay(500);

  Install_StandardStreams();

  printf("[me_Ws2812b.ino]\n");
  printf("Serial speed is %lu bps.\n", SerialSpeed);
  delay(500);
}

void loop()
{
  // Choose one of the tests:

  // Test_ObserveBitsTiming();
  // Test_ObserveColorsOrder();
  Test_WhiteSine();
  // Test_ColorSmoothing();
}

// --

/*
  Send several specific values to check timing margins with oscilloscope.
*/
void Test_ObserveBitsTiming()
{
  TUint_1 TestPacket[] =
    {
      B00000000,
      B1111111,
      // I want to see bit transitions 00, 01, 11, 10 (Gray codes huh):
      B0011010,
    };

  me_Ws2821b::SendPacket(TestPacket, sizeof(TestPacket), LedStripePin);

  delay(5000);
}

// --

/*
  Send bytes to observe colors order in stripe.

  Core function is just transmitting bytes, not colors. Well, triples
  of bytes. But color ordering is some permutation of RGB.

  So we will send (0xFF, 0, 0) to first pixel and (0, 0, 0xFF) to
  last pixel. Observe two colors. Can deduce remained color component
  from them.
*/
void Test_ObserveColorsOrder()
{
  const TUint_2 NumLeds = 3 * NumPixels;
  TUint_1 TestPacket[NumLeds];
  const TUint_2 TestPacket_Size = sizeof(TestPacket);

  memset(TestPacket, 0, TestPacket_Size);

  TestPacket[0] = 0xFF;
  TestPacket[1] = 0x00;
  TestPacket[2] = 0x00;

  TestPacket[TestPacket_Size - 3] = 0x00;
  TestPacket[TestPacket_Size - 2] = 0x00;
  TestPacket[TestPacket_Size - 1] = 0xFF;

  me_Ws2821b::SendPacket(TestPacket, TestPacket_Size, LedStripePin);

  delay(5000);
}

// --

/*
  Send rolling white sine wave.
*/
void Test_WhiteSine()
{
  const TUint_2 Delay_Ms = 20;

  using namespace me_Ws2821b;

  TPixel Pixels[NumPixels];

  static TUint_2 BaseAngle_Deg = 0;
  const TUint_1 BaseAngleShift_Deg = 1;
  const TUint_1 AngleIncrement_Deg = 6;

  TUint_2 Angle_Deg = BaseAngle_Deg;

  for (TUint_1 PixelIdx = 0; PixelIdx < NumPixels; ++PixelIdx)
  {
    const TUint_1
      ByteFloor = 0,
      ByteCeil = 120;

    TUint_1 ByteSine = (sin(DegToRad(Angle_Deg)) + 1) / 2 * 255;

    TUint_1 EtherValue = map(ByteSine, 0, 255, ByteFloor, ByteCeil);

    /*
    printf(
      "Angle: %3u, ByteSine: %3u, Ether: %3u\n",
      Angle_Deg,
      ByteSine,
      EtherValue
    );
    */

    Pixels[PixelIdx] =
      {
        .Green = EtherValue,
        .Red = EtherValue,
        .Blue = EtherValue,
      };

    Angle_Deg = (Angle_Deg + AngleIncrement_Deg) % 360;
  }

  BaseAngle_Deg = (BaseAngle_Deg + BaseAngleShift_Deg) % 360;

  SendPixels(Pixels, NumPixels, LedStripePin);

  delay(Delay_Ms);
}

// --

TUint_1 MapColorComponent(
  TUint_1 PixelIdx,
  TUint_2 NumPixels,
  TUint_1 StartColorValue,
  TUint_1 EndColorValue
)
{
  return
    map(
      PixelIdx,
      0,
      NumPixels - 1,
      StartColorValue,
      EndColorValue
    );
}

me_Ws2821b::TColor MapColor(
  TUint_1 PixelIdx,
  TUint_2 NumPixels,
  me_Ws2821b::TColor StartColor,
  me_Ws2821b::TColor EndColor
)
{
  me_Ws2821b::TColor Result;

  Result.Red =
    MapColorComponent(PixelIdx, NumPixels, StartColor.Red, EndColor.Red);
  Result.Green =
    MapColorComponent(PixelIdx, NumPixels, StartColor.Green, EndColor.Green);
  Result.Blue =
    MapColorComponent(PixelIdx, NumPixels, StartColor.Blue, EndColor.Blue);

  return Result;
}

/*
  Smooth transition from <StartColor> to <EndColor> and send it.
*/
void Test_ColorSmoothing()
{
  using namespace me_Ws2821b;

  TColor StartColor = { .Green = 255, .Red = 192, .Blue = 0, };
  TColor EndColor = { .Green = 32, .Red = 0, .Blue = 255, };

  TPixel Pixels[NumPixels];

  for (TUint_1 PixelIdx = 0; PixelIdx < NumPixels; ++PixelIdx)
  {
    Pixels[PixelIdx] = MapColor(PixelIdx, NumPixels, StartColor, EndColor);
  }

  SendPixels(Pixels, NumPixels, LedStripePin);

  delay(5000);
}

// --

/*
  2024-03-12
  2024-03-22
  2024-03-23
  2024-03-25
  2024-04-04
*/
