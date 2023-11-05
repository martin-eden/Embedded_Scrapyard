// <SoftwareSerial.h> test for Arduinoi Uno.

/*
  Status: stable
  Version: 1
  Last mod.: 2023-11-05
*/

#include <SoftwareSerial.h>

#include <me_Install_StandardStreams.h>

const uint8_t
  RX_Pin = 12,
  TX_Pin = 13;

SoftwareSerial SoftwareSerial_(RX_Pin, TX_Pin);

void setup()
{
  const uint32_t
    SoftwareSerialBaud = 9600,
    HardwareSerialBaud = 9600;

  Install_StandardStreams();

  Serial.begin(HardwareSerialBaud);

  SoftwareSerial_.begin(SoftwareSerialBaud);

  printf_P(
    PSTR(
      "--------------------------------------------------------------\n"
      "\n"
      "  Software serial test begins.\n"
      "\n"
      "    Software serial:\n"
      "\n"
      "      Baud: %lu\n"
      "      RX pin: %d\n"
      "      TX pin: %d\n"
      "\n"
      "    Hardware serial:\n"
      "\n"
      "      Baud: %lu\n"
      "\n"
      "    Protocol:\n"
      "\n"
      "      * Bytes we read from him, we print.\n"
      "      * Bytes you are sending to us, we giving a copy to him.\n"
      "--------------------------------------------------------------\n"
    ),
    SoftwareSerialBaud,
    RX_Pin,
    TX_Pin,
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
