#include "Motors.h"

#include <SoftwareSerial.h>

auto& usbSerial = Serial;
EspSoftwareSerial::UART softSerial;

void SetupMotorboardCommunication()
{
  Serial.println("Motorboard initialization.");

  // To do: Try reducing baud. Looks like 115200 messages arrive mangled.
  const uint32_t Baud = 9600; // 57600; // 115200;
  EspSoftwareSerial::Config Config = SWSERIAL_8N1;
  const uint8_t
    RX_Pin = D7,
    TX_Pin = D9;
  bool Invert = false;

  softSerial.begin(Baud, Config, RX_Pin, TX_Pin, Invert);
  if (!softSerial)
  {
    Serial.println("Software serial initialization failed.");
    while (1) delay (1000);
  }

  Serial.println("Motorboard initialized.");
}

void MotorsTest()
{
  Serial.println("Motor test.");

  // Send commands to motor board to briefly move motors.
  softSerial.write("L20R20D100 L0R0D200");
  delay(700);
  softSerial.write("L40R40D100 L0R0D200");
  delay(700);
  softSerial.write("L60R60D100 L0R0D200");
  delay(700);
  softSerial.write("L80R80D100 L0R0D200");
  delay(700);
  softSerial.write("L100R100D100 L0R0D200");
  delay(700);
  softSerial.write("L-20R-20D100 L0R0D200");
  delay(700);
  softSerial.write("L-40R-40D100 L0R0D200");
  delay(700);
  softSerial.write("L-60R-60D100 L0R0D200");
  delay(700);
  softSerial.write("L-80R-80D100 L0R0D200");
  delay(700);
  softSerial.write("L-100R-100D100 L0R0D200");
  delay(700);

  softSerial.write("L100R100D100 L0R0D200");
  delay(700);

  Serial.println("Motor test done.");
}
