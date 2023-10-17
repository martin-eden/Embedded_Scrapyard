// Firmware for my Rover v4.

/*
  Status: writing
  Version: 4
  Last mod.: 2023-10-17
*/

/*
  This is code for Wemos D1 connected to Arduino Uno (with motor board)
  via software serial.
*/

/*
  I've got troubles using <ESP8266TimerInterrupt.h> and Deek motor
  shield stacked top on board. When I start using timer interrupt,
  D2 goes high and D2 is Motor A PWM pin.

  As a workaround, NEW Arduino Uno BOARD added just for Deek motor
  shield. This Wemos is now _overseer_ board.

  It is connected to Arduino via software serial. So it controls motors
  by sending string commands to Arduino.

  Also it is connected to accelerometer on board. So intention is
  to measure and transmit acceleration in the time motor command is
  executed.
*/

/*
  2: Overseer

    Board: WeMos D1 R1
    Processor: ESP8266
    Accelerometer: MPU6050 (I2C)
*/

#include <ESP8266WiFi.h>

#include <WiFiUdp.h>

#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include <ESP8266TimerInterrupt.h>

#include <me_GyroAcc_MPU6050.h>

const char
  * NetworkName = "",
  * Password = "";

const uint16_t
  UdpPort = 31415;

const uint32_t
  SerialSpeed = 115200;

const uint32_t
  GyroPollInterval_Ms = 15000,
  TickTime_Ms = 50;

MPU6050::t_GyroAcc GyroAcc;

WiFiUDP Udp;
ESP8266WebServer Http(80);
ESP8266Timer Timer;

void DoMotorsTest()
{
  // Motor test will send commands to motor board to briefly move motors.
  Serial.println("There will be motor test.");
}

void SetupSerial()
{
  Serial.begin(SerialSpeed);
  delay(300);
}

void SetupGyro()
{
  if (!GyroAcc.Initialize())
  {
    Serial.println("Failed to initialize gyro.");
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

void Http_HandleRoot()
{
  Http.send(200, "text/plain", "Hello from rover-4-overseer-with-accelerometer!\n");
}

void Http_HandleNotFound()
{
  String message = "Not Found\n\n";
  message += "URI: ";
  message += Http.uri();
  message += "\nMethod: ";
  message += (Http.method() == HTTP_GET) ? "GET" : "POST";

  message += "\nArguments: ";
  message += Http.args();
  message += "\n";
  for (uint8_t i = 0; i < Http.args(); ++i)
  {
    message += " " + Http.argName(i) + ": " + Http.arg(i) + "\n";
  }

  Http.send(404, "text/plain", message);
}

void SetupHttp()
{
  Http.on("/", Http_HandleRoot);
  Http.onNotFound(Http_HandleNotFound);
  Http.begin();
}

void PrintSetupGreeting()
{
  Serial.println();
  Serial.println("---------------------------------------------------");
  Serial.println(" Rover-4 overseer with accelerometer: Hello there! ");
  Serial.println("---------------------------------------------------");
}

void PrintLoopGreeting()
{
  Serial.println("Rover-4 overseer with accelerometer: Loop here!");
}

void ProcessCommand()
{
  char PacketData[508];
  uint32_t PacketLength = Udp.parsePacket();
  if (PacketLength > 0)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", PacketLength, Udp.remoteIP().toString().c_str(), Udp.remotePort());

    uint32_t PacketLength2 = Udp.read(PacketData, sizeof(PacketData));
    PacketData[PacketLength2] = 0;

    Serial.printf("UDP packet contents: %s\n", PacketData);

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("Got your data.\n");
    Udp.endPacket();

    DoMotorsTest();
  }
}

void GetGyroReadings()
{
  MPU6050::t_GyroAccReadings GyroAccReadings;
  GyroAccReadings = GyroAcc.GetReadings();

  Serial.print("Acceleration");
  Serial.print("(");
  Serial.print(GyroAccReadings.Acceleration_Mps.x);
  Serial.print(" ");
  Serial.print(GyroAccReadings.Acceleration_Mps.y);
  Serial.print(" ");
  Serial.print(GyroAccReadings.Acceleration_Mps.z);
  Serial.print(")");
  Serial.print(" ");
  Serial.print("Rotation");
  Serial.print("(");
  Serial.print(GyroAccReadings.Rotation_Dps.x);
  Serial.print(" ");
  Serial.print(GyroAccReadings.Rotation_Dps.y);
  Serial.print(" ");
  Serial.print(GyroAccReadings.Rotation_Dps.z);
  Serial.print(")");
  Serial.print(" ");
  Serial.print("Temperature");
  Serial.print("(");
  Serial.print(GyroAccReadings.Temperature_C);
  Serial.print(")");
  Serial.print("\n");
}

void IRAM_ATTR GyroReadingsIsr()
{
  GetGyroReadings();
}

void setup()
{
  SetupSerial();
  PrintSetupGreeting();

  SetupGyro();

  SetupWiFi();

  SetupUdp();

  SetupHttp();

  Timer.attachInterruptInterval(GyroPollInterval_Ms * 1000, GyroReadingsIsr);

  DoMotorsTest();
}

void loop()
{
  // PrintLoopGreeting();

  ProcessCommand();

  Http.handleClient();

  delay(TickTime_Ms);
}

/*
  2023-10-07
  2023-10-08
  2023-10-14
  2023-10-17
*/
