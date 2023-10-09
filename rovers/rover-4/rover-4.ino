// Firmware for my Rover v4.

/*
  Status: writing
  Version: 1
  Last mod.: 2023-10-08
*/

/*
  Board: WeMos D1 ESP WROOM WiFi UNO
  Processor: ESP 8266
  Motor board: Deek
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <me_DeekMotor.h>
// #include <me_TwoDcMotorsDirector.h>

const uint8_t
  Deek_DirA_Pin = D12,
  Deek_PwmA_Pin = D3,
  Deek_BrakeA_Pin = D8,

  Deek_DirB_Pin = D13,
  Deek_PwmB_Pin = D11,
  Deek_BrakeB_Pin = D9;

const char
  * NetworkName = "",
  * Password = "";

const uint16_t
  UdpPort = 31415;

const uint32_t
  SerialSpeed = 115200;

const uint32_t
  TickTime_Ms = 50;

const TDeekMotorPins
  LeftMotorPins =
    {
      Direction_Pin: Deek_DirA_Pin,
      Pwm_Pin: Deek_PwmA_Pin,
      Brake_Pin: Deek_BrakeA_Pin
    },
  RightMotorPins =
    {
      Direction_Pin: Deek_DirB_Pin,
      Pwm_Pin: Deek_PwmB_Pin,
      Brake_Pin: Deek_BrakeB_Pin
    };

DeekMotor LeftMotor(LeftMotorPins);
DeekMotor RightMotor(RightMotorPins);
WiFiUDP Udp;

void DoMotorsTest()
{
  for (int16_t angle_deg = 0; angle_deg < 360; angle_deg = angle_deg + 4)
  {
    int8_t DesiredSpeed;

    float MagnifiedSine = sin(DEG_TO_RAD * angle_deg) * 100;

    DesiredSpeed = MagnifiedSine;

    LeftMotor.SetDesiredSpeed(DesiredSpeed);
    RightMotor.SetDesiredSpeed(DesiredSpeed);

    for (uint8_t i = 0; i < 4; ++i)
    {
      delay(2);
      LeftMotor.Update();
      RightMotor.Update();
    }
  }

  LeftMotor.SetDesiredSpeed(0);
  RightMotor.SetDesiredSpeed(0);
  for (uint8_t i = 0; i < 16; ++i)
  {
    delay(2);
    LeftMotor.Update();
    RightMotor.Update();
  }
}

void SetupWiFi()
{
  WiFi.begin(NetworkName, Password);

  int8_t ConnectionStatus = WiFi.waitForConnectResult();
  if (ConnectionStatus == WL_CONNECTED)
  {
    Serial.println("Connected!");
    Serial.println();

    Serial.printf("Hostname: %s\n", WiFi.hostname().c_str());

    Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("DNS: ");
    WiFi.dnsIP().printTo(Serial);
    Serial.println();

    Serial.printf("Station: %s\n", WiFi.SSID().c_str());
    Serial.printf("Password: %s\n", WiFi.psk().c_str());
    Serial.printf("Station MAC: %s\n", WiFi.BSSIDstr().c_str());
    Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
  }
}

void SetupUdp()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Udp.begin(UdpPort);
    Serial.print("Started UDP on port ");
    Serial.print(UdpPort);
    Serial.println(".");
  }
}

void PrintSetupGreeting()
{
  Serial.println();
  Serial.println("---------------------");
  Serial.println("Rover-4: Hello there!");
  Serial.println("---------------------");
}

void PrintLoopGreeting()
{
  Serial.println("-------------------");
  Serial.println("Rover-4: Loop here!");
  Serial.println("-------------------");
}

void ProcessCommand()
{
  char PacketData[508];
  char ReplyPacket[] = "Got your data.";

  uint32_t PacketLength = Udp.parsePacket();
  if (PacketLength > 0)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", PacketLength, Udp.remoteIP().toString().c_str(), Udp.remotePort());

    uint32_t PacketLength2 = Udp.read(PacketData, sizeof(PacketData));
    PacketData[PacketLength2] = 0;

    Serial.printf("UDP packet contents: %s\n", PacketData);

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("Got your data.");
    Udp.write('\n');
    Udp.write(PacketLength2);
    Udp.endPacket();

    DoMotorsTest();
  }
}

void setup()
{
  Serial.begin(SerialSpeed);

  PrintSetupGreeting();
  SetupWiFi();
  SetupUdp();
  DoMotorsTest();
}

void loop()
{
  // PrintLoopGreeting();
  ProcessCommand();
  delay(TickTime_Ms);
}

/*
  2023-10-07
  2023-10-08
*/
