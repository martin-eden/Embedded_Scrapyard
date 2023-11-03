// Firmware for my Rover v4.

/*
  Status: working base
  Version: 7
  Last mod.: 2023-11-02
*/

/*
  2: Overseer

    Board: WeMos D1 R1
    Processor: ESP8266
    Accelerometer: MPU6050 (I2C)
*/

/*
  I've got troubles using Deek motor shield for Arduino Uno on Wemos D1.

  So motor shield got dedicated Arduino Uno and Wemos sitting alone,
  with WiFi, connected to Uno UART and responsible for communications
  between boards and outer world.

  To make life of Wemos more interesting, I've connected accelerometer
  (via I2C). So it may have IMU-related API endpoints and sync them
  with sending command to motor board.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <ArduinoJson.h>

#include <me_GyroAcc_MPU6050.h>

const char
  * StationName = "",
  * StationPassword = "";

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

bool IsGyroInitialized = false;
const uint16_t GyroHistoryCapacity = 150; // 150;
t_GyroHistoryRec GyroHistory[GyroHistoryCapacity];
uint16_t GyroHistoryCursor = 0;

const uint16_t Http_Response_Ok = 200;
const uint16_t Http_Response_NotFound = 404;
const char * Http_Content_Plaintext = "text/plain";

ESP8266WebServer Http;

Ticker Timer;

void SetupSerial()
{
  Serial.begin(SerialSpeed);
  delay(300);
}

void PrintOurBanner()
{
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println(" Rover-4 overseer with accelerometer ");
  Serial.println("-------------------------------------");
}

void SetupGyro()
{
  Serial.print("Initializing gyro..");
  for (
    uint8_t NumTries = 0;
    (NumTries < 10) && !IsGyroInitialized;
    ++NumTries
  )
  {
    IsGyroInitialized = GyroAcc.Initialize();
    delay(100);
    Serial.print(".");
  }

  Serial.print(" ");

  if (!IsGyroInitialized)
  {
    Serial.println("nah.");
  }
  else
  {
    Serial.println("yep.");
  }
}

void SetupWiFi(char const * _StationName, char const * _StationPassword)
{
  Serial.println("Setting-up WiFi: [");

  Serial.printf(
    "  We are:\n"
    "    Name: %s\n"
    "    MAC: %s\n"
    "  Connecting to:\n"
    "    Station: %s\n"
    "    .. with some password\n",
    WiFi.hostname().c_str(),
    WiFi.macAddress().c_str(),
    _StationName
  );

  uint32_t StartTimeMs = millis();

  WiFi.begin(_StationName, _StationPassword);

  int8_t ConnectionStatus = WiFi.waitForConnectResult(32000);

  uint32_t FinishTimeMs = millis();
  uint32_t TimePassedMs = FinishTimeMs - StartTimeMs;

  switch (ConnectionStatus)
  {
    case WL_CONNECTED:
      Serial.println(
        "  Connected:"
      );
      Serial.println(
        "    Station:"
      );
      Serial.printf(
        "      RSSI (dBm): %d\n", WiFi.RSSI()
      );
      Serial.printf(
        "      MAC: %s\n", WiFi.BSSIDstr().c_str()
      );
      Serial.println(
        "    Our:"
      );
      Serial.print(
        "      IP: "
      );
      Serial.println(WiFi.localIP());
      Serial.print(
        "      DNS: "
      );
      Serial.println(WiFi.dnsIP());

      break;

    case WL_NO_SSID_AVAIL:
      Serial.println("  Can't see station.");
      break;

    case WL_WRONG_PASSWORD:
      Serial.println("  Wrong password.");
      break;

    case WL_CONNECT_FAILED:
      Serial.println("  I see station but connection failed. Probably wrong password.");
      break;

    case NA_STATE:
      Serial.println("  We finished too early. Connection has not came.");
      break;

    default:
      Serial.printf("  [debug] Uncovered case %d.\n", ConnectionStatus);
      break;
  }

  Serial.printf("  Time taken (ms): %u\n", TimePassedMs);

  Serial.println("]");
}

void SetupHttp()
{
  Serial.println("Setting-up HTTP: [");

  Http.on("/", Http_HandleRoot);
  Serial.println("  Added / hook.");

  Http.onNotFound(Http_HandleNotFound);
  Serial.println("  Added NOT_FOUND hook.");

  Http.begin();

  Serial.println("]");
}

void GyroPoll_Isr();

void SetupIsr()
{
  Timer.attach_ms(GyroPollInterval_Ms, GyroPoll_Isr);

  Serial.printf("Set-up timer interrupt every %d ms.\n", GyroPollInterval_Ms);
}

void Http_HandleRoot()
{
  String GyroReadings_Str = SerializeGyroReadings(LastGyroReadings, LastGyroReadingsTime_Ms);

  Http.send(Http_Response_Ok, Http_Content_Plaintext, GyroReadings_Str);

  Serial.printf("[%lu] Sent gyro readings to ", millis());
  Serial.println(Http.client().remoteIP());
}

void Http_HandleNotFound()
{
  Http.send(Http_Response_NotFound, Http_Content_Plaintext, "Not Found\n\n");
}

/*
  Get gyro readings (acceleration, rotation, temperature) and process
  them before returning.

  Values are rounded to some granularity value.

  This is done to reduce minor fluctuations in values (jitter).

  For cases when value is between two granulated points, jitter still
  persists for and just amplified to granularity value. (Fluctuations
  (1.49, 1.51, 1.49, ...) are rounded to (1.0, 2.0, 1.0, ...).)

  Jitter can be reduced by capacitive filtering before granulating but
  current implementation is good enough for now.
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

void IRAM_ATTR GyroPoll_Isr()
{
  LastGyroReadings = GetGyroReadings();
  LastGyroReadingsTime_Ms = millis();

  StoreGyroReadings(LastGyroReadings, LastGyroReadingsTime_Ms);
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

void PrintLoopGreeting()
{
  Serial.printf("[%lu] Main loop iteration.", millis());
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

  // PrintGyroHistory();
}

void setup()
{
  SetupSerial();

  PrintOurBanner();

  SetupGyro();

  SetupWiFi(StationName, StationPassword);

  SetupHttp();

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
