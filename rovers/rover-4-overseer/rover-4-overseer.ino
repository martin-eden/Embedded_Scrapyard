// Firmware for my Rover v4.

/*
  Status: working base
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

#include <ESP8266WebServer.h>

#include <ESP8266TimerInterrupt.h>

#include <me_GyroAcc_MPU6050.h>

const char
  * NetworkName = "",
  * Password = "";

const uint32_t
  SerialSpeed = 115200;

const uint32_t
  GyroPollInterval_Ms = 10,
  TickTime_Ms = 50;

MPU6050::t_GyroAcc GyroAcc;
MPU6050::t_GyroAccReadings LastGyroReadings;

ESP8266WebServer Http;
ESP8266Timer Timer;

void SetupSerial()
{
  Serial.begin(SerialSpeed);
  delay(300);
}

void IRAM_ATTR GyroReadingsIsr()
{
  LastGyroReadings = GetGyroReadings();

  // PrintGyroReadings(LastGyroReadings);
}

void SetupGyro()
{
  if (!GyroAcc.Initialize())
  {
    Serial.println("Failed to initialize gyro.");
    Serial.println("Stopped.");
    while (1);
  }

  Timer.attachInterruptInterval(GyroPollInterval_Ms * 1000, GyroReadingsIsr);
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

    Serial.println();
  }
  else
  {
    Serial.println("Not able to connect WiFi.");
    Serial.println("Stopped.");
    while (1);
  }
}

void SetupHttp()
{
  Http.on("/", Http_HandleRoot);
  Http.onNotFound(Http_HandleNotFound);
  Http.begin();
}

void DoMotorsTest()
{
  // Motor test will send commands to motor board to briefly move motors.
  Serial.println("Here will be motor test.");
}

void Http_HandleRoot()
{
  const uint16_t
    Response_Ok = 200;
  const char
    * Content_Plaintext = "text/plain";

  String SerializedGyroReadings = SerializeGyroReadings(LastGyroReadings);

  Http.send(Response_Ok, Content_Plaintext, SerializedGyroReadings);
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

void PrintSetupGreeting()
{
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println(" Rover-4 overseer with accelerometer ");
  Serial.println("-------------------------------------");
}

void PrintLoopGreeting()
{
  Serial.println("Rover-4 overseer with accelerometer: Loop here!");
}

void ProcessCommand()
{
  Http.handleClient();
}

String SerializeGyroReadings(MPU6050::t_GyroAccReadings GyroReadings)
{
  String Result = "";

  Result += "Acceleration";
  Result += "(";
  Result += GyroReadings.Acceleration_Mps.x;
  Result += " ";
  Result += GyroReadings.Acceleration_Mps.y;
  Result += " ";
  Result += GyroReadings.Acceleration_Mps.z;
  Result += ")";
  Result += " ";
  Result += "Rotation";
  Result += "(";
  Result += GyroReadings.Rotation_Dps.x;
  Result += " ";
  Result += GyroReadings.Rotation_Dps.y;
  Result += " ";
  Result += GyroReadings.Rotation_Dps.z;
  Result += ")";
  Result += " ";
  Result += "Temperature";
  Result += "(";
  Result += GyroReadings.Temperature_C;
  Result += ")";
  Result += "\n";

  return Result;
}

void PrintGyroReadings(MPU6050::t_GyroAccReadings GyroReadings)
{
  Serial.print(SerializeGyroReadings(GyroReadings));
}

MPU6050::t_GyroAccReadings GetGyroReadings()
{
  MPU6050::t_GyroAccReadings GyroReadings;

  GyroReadings = GyroAcc.GetReadings();

  return GyroReadings;
}

void setup()
{
  SetupSerial();

  PrintSetupGreeting();

  SetupGyro();

  SetupWiFi();
  SetupHttp();

  // DoMotorsTest();
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
  2023-10-14
  2023-10-17
*/
