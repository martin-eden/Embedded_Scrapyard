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
#include <Ticker.h>

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
uint32_t LastGyroReadingsTime_Ms = 0;

ESP8266WebServer Http;

Ticker Timer;

void SetupSerial()
{
  Serial.begin(SerialSpeed);
  delay(300);
}

void SetupGyro()
{
  bool GyroInitialized = false;
  for (
    uint8_t NumTries = 0;
    (NumTries < 20) && !GyroInitialized;
    ++NumTries
  )
  {
    GyroInitialized = GyroAcc.Initialize();
    delay(100);
  }

  if (!GyroInitialized)
  {
    Serial.println("Failed to initialize gyro.");
    Serial.println("Stopped.");
    while (1);
  }
  else
  {
    Serial.println("Gyro initialized.");
  }
}

MPU6050::t_GyroAccReadings GetGyroReadings()
{
  return GyroAcc.GetReadings();
}

void SetupWiFi()
{
  WiFi.begin(NetworkName, Password);

  int8_t ConnectionStatus = WiFi.waitForConnectResult();
  if (ConnectionStatus == WL_CONNECTED)
  {
    Serial.println("Connected to WiFi.");
    Serial.println();

    Serial.printf("  Hostname: %s\n", WiFi.hostname().c_str());

    Serial.printf("  MAC: %s\n", WiFi.macAddress().c_str());

    Serial.print("  IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("  DNS: ");
    WiFi.dnsIP().printTo(Serial);
    Serial.println();

    Serial.printf("  Station: %s\n", WiFi.SSID().c_str());
    Serial.printf("  Password: %s\n", WiFi.psk().c_str());
    Serial.printf("  Station MAC: %s\n", WiFi.BSSIDstr().c_str());
    Serial.printf("  RSSI: %d dBm\n", WiFi.RSSI());

    Serial.println();
  }
  else
  {
    Serial.println("Unable to connect to WiFi.");
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

void IRAM_ATTR GyroPoll_Isr()
{
  LastGyroReadings = GetGyroReadings();
  LastGyroReadingsTime_Ms = millis();
}

void SetupIsr()
{
  Timer.attach_ms(GyroPollInterval_Ms, GyroPoll_Isr);
}

String SerializeGyroReadings(MPU6050::t_GyroAccReadings GyroReadings, uint32_t Time)
{
  String Result = "";

  Result += "Time";
  Result += "(";
  Result += Time;
  Result += ")";
  Result += " ";

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

  return Result;
}

void DoMotorsTest()
{
  // Motor test will send commands to motor board to briefly move motors.
  Serial.println("Here will be motor test.");
}

void Http_HandleRoot()
{
  const uint16_t Response_Ok = 200;
  const char * Content_Plaintext = "text/plain";

  String GyroReadings_Str = SerializeGyroReadings(LastGyroReadings, LastGyroReadingsTime_Ms);

  Http.send(Response_Ok, Content_Plaintext, GyroReadings_Str);

  Serial.println(GyroReadings_Str);
}

void Http_HandleNotFound()
{
  Http.send(404, "text/plain", "Not Found\n\n");
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

void DoBusiness()
{
  Http.handleClient();

  // Serial.print(SerializeGyroReadings(LastGyroReadings));
  // Serial.println();
}

void setup()
{
  SetupSerial();

  PrintSetupGreeting();

  SetupWiFi();
  SetupHttp();

  SetupGyro();

  SetupIsr();

  // DoMotorsTest();
}

void loop()
{
  // PrintLoopGreeting();

  DoBusiness();

  delay(TickTime_Ms);
}

/*
  2023-10-07
  2023-10-08
  2023-10-14
  2023-10-17
*/
