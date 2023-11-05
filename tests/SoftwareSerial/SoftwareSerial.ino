// <SoftwareSerial.h> test for Arduinoi Uno.

/*
  Status: stable
  Version: 1
  Last mod.: 2023-11-05
*/

#include <SoftwareSerial.h>

const uint8_t
  RX_Pin = 12,
  TX_Pin = 13;

SoftwareSerial SoftwareSerial_(RX_Pin, TX_Pin);

void PrintfSetup();

void setup()
{
  const uint32_t
    SoftwareSerialBaud = 9600,
    HardwareSerialBaud = 9600;

  PrintfSetup();

  Serial.begin(HardwareSerialBaud);

  SoftwareSerial_.begin(SoftwareSerialBaud);

  printf(
    "--------------------------------------------------------------\n"
    "\n"
    "  Software serial test begins.\n"
    "\n"
    "    Software serial baud: %lu\n"
    "\n"
    "    Our baud is %lu.\n"
    "\n"
    "    Protocol:\n"
    "\n"
    "      * Bytes we read from him, we print.\n"
    "      * Bytes you are sending to us, we giving a copy to him.\n"
    "--------------------------------------------------------------\n",
    SoftwareSerialBaud,
    HardwareSerialBaud
  );
}

void loop()
{
  static char SoftwareSerialCharRead, HardwareSerialCharRead;

  while(SoftwareSerial_.available()) // SoftwareSerial_ has data
  {
    SoftwareSerialCharRead = SoftwareSerial_.read();

    Serial.write(SoftwareSerialCharRead);
  }

  while(Serial.available()) // Our Serial has data
  {
    HardwareSerialCharRead = Serial.read();

    SoftwareSerial_.write(HardwareSerialCharRead);
  }
}

FILE f_out;

int putc_serial(char c, FILE *)
{
  bool Result = Serial.write(c);
  return Result;
}

void PrintfSetup()
{
  fdevopen(&putc_serial, 0);
}
