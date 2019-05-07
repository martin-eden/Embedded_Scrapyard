#include <SoftwareSerial.h>

const uint8_t
 bt_rxPin = 3,
 bt_txPin = 2;

const uint16_t
 bt_baudrate = 9600;

String msg;
SoftwareSerial hc05(bt_rxPin, bt_txPin);

void setup()
{
  pinMode(bt_rxPin, INPUT);
  pinMode(bt_txPin, OUTPUT);

  Serial.begin(9600);
  Serial.println("ENTER AT Commands:");

  hc05.begin(bt_baudrate);
}

void loop()
{
  readSerialPort();

  if (msg != "")
  {
    hc05.println(msg);
    Serial.println(msg);
  }

  while (hc05.available() > 0)
  {
    Serial.write(hc05.read());
  }
}

void readSerialPort()
{
  msg = "";
  while (Serial.available())
  {
    delay(10);
    if (Serial.available() > 0)
    {
      char c = Serial.read();
      msg += c;
    }
  }
}
