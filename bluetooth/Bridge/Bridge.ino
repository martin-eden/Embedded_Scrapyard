// Send AT commands to HC05 Bluetooth module.

/*
  Status: works
  Version: 2
  Last mod.: 2023-05-23
*/

/*
  Keep pressing that tiny button on HC05 and connect to VCC.
  LED should start blinking near 0.5Hz. That's the sacred Command Mode.

  Some commands like AT+INQ, AT+NAME?, AT+CLASS? requires pressing
  that tiny button.

  AT+INIT kicks HC05 back to Data Mode.
*/

#include <SoftwareSerial.h>

const uint8_t
 HC05_TX_pin = 2,
 HC05_RX_pin = 3;

const uint16_t
 SerialBaudrate = 57600,

 /*
   Use 38400 for command mode, and any baud you cofigured via AT+UART
   for echo test.
*/

 BluetoothBaudrate = 57600;

SoftwareSerial HC05(HC05_TX_pin, HC05_RX_pin);

void setup()
{
  HC05.begin(BluetoothBaudrate);

  Serial.begin(SerialBaudrate);
}

void loop()
{
  if (Serial.available())
  {
    while (Serial.available())
    {
      char c = Serial.read();
      HC05.write(c);
    }
  }

  if (HC05.available())
  {
    while (HC05.available())
    {
      char c = HC05.read();
      Serial.write(c);
    }
  }
}
