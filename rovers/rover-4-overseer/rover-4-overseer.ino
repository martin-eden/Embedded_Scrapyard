// Firmware for my Rover v4.

/*
  Status: working base
  Version: 6
  Last mod.: 2023-10-25
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
#include <ArduinoJson.h>

#include <me_GyroAcc_MPU6050.h>

const char
  * NetworkName = "",
  * Password = "";

const uint32_t
  SerialSpeed = 115200;

const uint32_t
  GyroPollInterval_Ms = 50,
  TickTime_Ms = 50;

MPU6050::t_GyroAcc GyroAcc;
MPU6050::t_GyroAccReadings LastGyroReadings;
uint32_t LastGyroReadingsTime_Ms = 0;

/*
  For gyro readings history we need timestamp and can store subset of
  gyro each gyro status to reduce memory consumption.
*/
struct t_GyroHistoryRec
{
  uint32_t Timestamp_Ms;
  struct
  {
    float x;
    float y;
    float z;
  } Acceleration_Mps;
};

const uint16_t GyroHistoryCapacity = 150; // 150;
t_GyroHistoryRec GyroHistory[GyroHistoryCapacity];
uint16_t GyroHistoryCursor = 0;

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
    (NumTries < 10) && !GyroInitialized;
    ++NumTries
  )
  {
    GyroInitialized = GyroAcc.Initialize();
    delay(200);
    Serial.print(".");
  }

  Serial.println();

  if (!GyroInitialized)
  {
    Serial.println("Failed to initialize gyro.");
    Serial.println("Stopped.");
    while (1);
  }
  else
  {
    // Serial.println("Gyro initialized.");
  }
}

/*
  Get gyro readings (acceleration, rotation, temeperature) and process
  them before returning.

  Values are rounded to some granularity value.

  This is done to reduce minor fluctuations in values (jitter).

  For cases when value is between two granulated points, jitter still
  remains for and just amplified to granularity value. (Fluctuations
  (1.49, 1.51, 1.49, ...) are rounded to (1, 2, 1, ...).)

  Jitter can be further reduced by capacitive filtering before
  granulating but current implementation is good enough for now.
*/
MPU6050::t_GyroAccReadings GetGyroReadings()
{
  // Granularity for float values.
  const float
    GranularityUnit_Mps = 0.25,
    GranularityUnit_Dps = 0.25,
    GranularityUnit_C = 0.25;

  MPU6050::t_GyroAccReadings Result;

  Result = GyroAcc.GetReadings();

  Result.Acceleration_Mps.x = RoundToUnit(Result.Acceleration_Mps.x, GranularityUnit_Mps);
  Result.Acceleration_Mps.y = RoundToUnit(Result.Acceleration_Mps.y, GranularityUnit_Mps);
  Result.Acceleration_Mps.z = RoundToUnit(Result.Acceleration_Mps.z, GranularityUnit_Mps);

  Result.Rotation_Dps.x = RoundToUnit(Result.Rotation_Dps.x, GranularityUnit_Dps);
  Result.Rotation_Dps.y = RoundToUnit(Result.Rotation_Dps.y, GranularityUnit_Dps);
  Result.Rotation_Dps.z = RoundToUnit(Result.Rotation_Dps.z, GranularityUnit_Dps);

  Result.Temperature_C = RoundToUnit(Result.Temperature_C, GranularityUnit_C);

  return Result;
}

void StoreGyroReadings(MPU6050::t_GyroAccReadings Readings, uint32_t Timestamp_Ms)
{
  t_GyroHistoryRec HistoryRec;

  HistoryRec.Acceleration_Mps.x = Readings.Acceleration_Mps.x;
  HistoryRec.Acceleration_Mps.y = Readings.Acceleration_Mps.y;
  HistoryRec.Acceleration_Mps.z = Readings.Acceleration_Mps.z;
  HistoryRec.Timestamp_Ms = Timestamp_Ms;

  GyroHistory[GyroHistoryCursor] = HistoryRec;

  ++GyroHistoryCursor;
  GyroHistoryCursor %= GyroHistoryCapacity;
}

String SerializeHistoryRec_Json(t_GyroHistoryRec HistoryRec)
{
  String Result = "";

  StaticJsonDocument<192> doc;

  doc["Timestamp_ms"] = HistoryRec.Timestamp_Ms;

  JsonObject Acceleration = doc.createNestedObject("Acceleration_v3_mps");
  Acceleration["X"] = HistoryRec.Acceleration_Mps.x;
  Acceleration["Y"] = HistoryRec.Acceleration_Mps.y;
  Acceleration["Z"] = HistoryRec.Acceleration_Mps.z;

  serializeJson(doc, Result);

  return Result;
}

void PrintWifiCredentials()
{
  Serial.printf("  Station: %s\n", WiFi.SSID().c_str());
  Serial.printf("  Password: %s\n", WiFi.psk().c_str());
  Serial.println();
}

void PrintWifiParameters()
{
  int8_t ConnectionStatus = WiFi.status();
  if (ConnectionStatus == WL_CONNECTED)
  {
    Serial.println("Connected to WiFi.");
    Serial.println();

    PrintWifiCredentials();

    Serial.printf("  Hostname: %s\n", WiFi.hostname().c_str());

    Serial.printf("  MAC: %s\n", WiFi.macAddress().c_str());

    Serial.print("  IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("  DNS: ");
    WiFi.dnsIP().printTo(Serial);
    Serial.println();

    Serial.printf("  Station MAC: %s\n", WiFi.BSSIDstr().c_str());
    Serial.printf("  RSSI: %d dBm\n", WiFi.RSSI());

    Serial.println();
  }
  else
  {
    Serial.println("Unable to connect to WiFi.");
    PrintWifiCredentials();
  }
}

void SetupWiFi()
{
  WiFi.begin(NetworkName, Password);

  int8_t ConnectionStatus = WiFi.waitForConnectResult();

  // To use data plotter I need option to print nothing at successfull connection.
  if (ConnectionStatus != WL_CONNECTED)
  {
    PrintWifiParameters();
  }

  if (ConnectionStatus != WL_CONNECTED)
  {
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

  StoreGyroReadings(LastGyroReadings, LastGyroReadingsTime_Ms);
}

void SetupIsr()
{
  Timer.attach_ms(GyroPollInterval_Ms, GyroPoll_Isr);
}

String SerializeGyroReadings(MPU6050::t_GyroAccReadings GyroReadings, uint32_t Time)
{
  String Result = "";
  StaticJsonDocument<192> doc;

  doc["Timestamp_ms"] = Time;

  JsonObject Acceleration_V3 = doc.createNestedObject("Acceleration_v3_mps");
  Acceleration_V3["X"] = GyroReadings.Acceleration_Mps.x;
  Acceleration_V3["Y"] = GyroReadings.Acceleration_Mps.y;
  Acceleration_V3["Z"] = GyroReadings.Acceleration_Mps.z;

  JsonObject Rotation_V3 = doc.createNestedObject("Rotation_v3_dps");
  Rotation_V3["X"] = GyroReadings.Rotation_Dps.x;
  Rotation_V3["Y"] = GyroReadings.Rotation_Dps.y;
  Rotation_V3["Z"] = GyroReadings.Rotation_Dps.z;
  doc["Temperature_C"] = GyroReadings.Temperature_C;

  serializeJson(doc, Result);

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

float RoundToUnit(float Value, float UnitSize)
{
  return trunc(Value / UnitSize) * UnitSize;
}

// Serialize gyro values for Arduino IDE plotter.
String SerializeHistoryRec_Tsv(t_GyroHistoryRec GyroHistoryRec)
{
  String Result = "";

  Result += GyroHistoryRec.Acceleration_Mps.x;
  Result += "\t";
  Result += GyroHistoryRec.Acceleration_Mps.y;
  Result += "\t";
  Result += GyroHistoryRec.Acceleration_Mps.z;
  // Result += " ";
  // Result += GyroHistoryRec.Temperature_C;

  return Result;
}

// Print history of gyro readings.
void PrintGyroHistory()
{
  const uint32_t PrintInterval_Ms = 5000;
  static uint32_t LastPrintTime_Ms = 0;
  uint32_t CurrentTime_Ms = millis();
  bool IsTimeToPrint = (CurrentTime_Ms - LastPrintTime_Ms >= PrintInterval_Ms);

  if (!IsTimeToPrint)
    return;

  for (
    uint16_t PrintCursor = GyroHistoryCursor + 1;
    PrintCursor != GyroHistoryCursor;
    PrintCursor = (PrintCursor + 1) % GyroHistoryCapacity
  ) {
    Serial.println(SerializeHistoryRec_Tsv(GyroHistory[PrintCursor]));
  }

  LastPrintTime_Ms = CurrentTime_Ms;
}

void DoLoopBusiness()
{
  Http.handleClient();

  PrintGyroHistory();
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

  DoLoopBusiness();

  delay(TickTime_Ms);
}

/*
  2023-10-07
  2023-10-08
  2023-10-14
  2023-10-17
  2023-10-21
*/
