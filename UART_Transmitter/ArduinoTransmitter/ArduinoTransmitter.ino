// Well, I dunno what it finally will be... WIP

/*
  Currently it's sort of SoftwareSerial test with ability to change
  pins we are listening and baud at runtime.
*/

/*
  Status: in writing
  Version: 0
  Last mod.: 2023-11-05
*/

#include <me_Install_StandardStreams.h>

#include "ArduinoTransmitter_internals.h"

const uint32_t
  HardwareSerial_Baud = 9600,
  SoftwareSerial_Baud = 10000;

const uint8_t
  SoftwareSerial_Rx_Pin = 8,
  SoftwareSerial_Tx_Pin = 9;

using namespace TransmitterInternals;

TransmitterInternals::UART_Transmitter Transmitter_;

void setup()
{
  Serial.begin(HardwareSerial_Baud);

  Install_StandardStreams();

  Transmitter_.ConnectTo(SoftwareSerial_Rx_Pin, SoftwareSerial_Tx_Pin, SoftwareSerial_Baud);

  if (Transmitter_.IsConnected())
  {
    printf(
      "Connected to (%d, %d, %lu)\n",
      SoftwareSerial_Rx_Pin, SoftwareSerial_Tx_Pin, SoftwareSerial_Baud
      );
  }
  else
  {
    printf(
      "Failed to connect to (%d, %d, %lu)\n",
      SoftwareSerial_Rx_Pin, SoftwareSerial_Tx_Pin, SoftwareSerial_Baud
      );
  }

  printf_P(
    PSTR(
      "\n"
      "--------------------------------------------------------------\n"
      "\n"
      "  UART transmitter test starts.\n"
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
      "\n"
      "--------------------------------------------------------------\n"
      "\n"
    ),
    SoftwareSerial_Baud,
    SoftwareSerial_Rx_Pin,
    SoftwareSerial_Tx_Pin,
    HardwareSerial_Baud
  );

}

void loop()
{
  static char SoftwareSerialCharRead;
  // static char HardwareSerialCharRead;

  while(Transmitter_.GetNumBytesAvailable()) // SoftwareSerial_ has data
  {
    SoftwareSerialCharRead = Transmitter_.ReadByte();

    Serial.write(SoftwareSerialCharRead);
  }

  /*
  while(Serial.available()) // Our Serial has data
  {
    HardwareSerialCharRead = Serial.read();

    Transmitter_.WriteByte(HardwareSerialCharRead);
  }
  */
}
