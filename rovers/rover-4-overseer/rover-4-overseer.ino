// Firmware for my Rover v4.

/*
  Status: working base
  Version: 10
  Last mod.: 2023-11-14
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
  with WiFi, connected to Uno UART and responsible for communication
  with motor board and with outer world over HTTP.

  To make life of Wemos more interesting, I've connected accelerometer
  via I2C. So now it may have accelerometer-related API endpoints
  (start logging, stop logging) and sync log acceleration during
  execution of motorboard command.
*/

#include <Ticker.h>

#include "Motorboard.h"

#include "Gyro.h"
#include "GyroHistory.h"

#include "me_Wifi.h"
#include "Http.h"

namespace Overseer
{
  const char
    * StationName = "",
    * StationPassword = "";

  const uint32_t
    Serial_Baud = 115200,
    Motorboard_Baud = 57600; // 9600; 57600; 115200;

  const uint8_t
    Motorboard_Receive_Pin = D7,
    Motorboard_Transmit_Pin = D9;

  const uint32_t
    GyroPollInterval_Ms = 1200,
    HeartbeatInterval_S = 60,
    TickTime_Ms = 50;

  constexpr auto *CommentStream = &Serial;

  Motorboard Motorboard_;

  Ticker GyroPoll_Timer;
  Ticker Heartbeat_Timer;

  void PrintLabel();
  void PrintSettings();

  void HttpRootHandler_Callback();
  void HttpRootHandler_Callback();

  void SetupGyroPoll(uint16_t Interval_Ms, Ticker::callback_function_t Callback);
  void GyroPoll_IsrCallback();

  void SetupHeartbeat(uint32_t Interval_S);
  void Heartbeat_Callback();
}

// ---

using namespace Overseer;

void setup()
{
  {
    uint16_t SerialWarmup_Ms = 300;
    CommentStream->begin(Serial_Baud);
    delay(SerialWarmup_Ms);
  }

  PrintLabel();

  PrintSettings();

  CommentStream->printf(
    "--[ Setup\n"
  );

  bool MotorboardIsConnected =
    Motorboard_.SetupConnection(
      Motorboard_Baud,
      Motorboard_Receive_Pin,
      Motorboard_Transmit_Pin
    );

  if (MotorboardIsConnected)
  {
    Motorboard_.RunMotorsTest();
  }

  bool GyroIsConnected = SetupGyro();

  uint16_t WifiConnectTimeout_S = 20;
  bool WifiIsConnected =
    me_Wifi::SetupWifi(
      StationName,
      StationPassword,
      WifiConnectTimeout_S
    );

  if (WifiIsConnected)
  {
    Http::Setup(HttpRootHandler_Callback);
  }

  if (GyroIsConnected)
  {
    SetupGyroPoll(GyroPollInterval_Ms, GyroPoll_IsrCallback);
  }

  SetupHeartbeat(HeartbeatInterval_S);

  CommentStream->printf(
    PSTR(
      "Modules\n"
      "\n"
      "  Motorboard = %u\n"
      "  Gyro = %u\n"
      "  Wi-Fi = %u\n"
      "\n"
    ),
    MotorboardIsConnected,
    GyroIsConnected,
    WifiIsConnected
  );

  CommentStream->printf(
    "--] Setup\n"
  );
}

void loop()
{
  Http::HandleEvents();

  delay(TickTime_Ms);
}

// ---

void Overseer::PrintLabel()
{
  CommentStream->printf(
    PSTR(
      "\n"
      "--------------------------------------\n"
      " Motorboard client with accelerometer \n"
      "--------------------------------------\n"
      "\n"
    )
  );
}

void Overseer::PrintSettings()
{
  CommentStream->printf(
    PSTR(
      "Settings\n"
      "\n"
      "  Our stream baud: %u\n"
      "\n"
      "  Motorboard connection\n"
      "\n"
      "    Baud: %u\n"
      "    Receive pin: %u\n"
      "    Transmit pin: %u\n"
      "\n"
      "  Accelerometer poll interval (ms): %u\n"
      "\n"
    ),
    Serial_Baud,
    Motorboard_Baud,
    Motorboard_Receive_Pin,
    Motorboard_Transmit_Pin,
    GyroPollInterval_Ms
  );
}

void Overseer::HttpRootHandler_Callback()
{
  String GyroReadings_Str =
    SerializeGyroReadings(GetLastGyroReadings(), GetLastGyroReadingsTime_Ms());

  Http::SendString(GyroReadings_Str);

  CommentStream->printf(
    "[%lu] Sent gyro readings to %s.\n",
    millis(),
    Http::GetClientIp().c_str()
  );
}

void Overseer::SetupGyroPoll(uint16_t Interval_Ms, Ticker::callback_function_t Callback)
{
  GyroPoll_Timer.attach_ms(Interval_Ms, Callback);
}

void IRAM_ATTR Overseer::GyroPoll_IsrCallback()
{
  UpdateGyroReadings();
  StoreGyroReadings(GetLastGyroReadings(), GetLastGyroReadingsTime_Ms());
}

void Overseer::SetupHeartbeat(uint32_t Interval_S)
{
  Heartbeat_Timer.attach_scheduled(Interval_S, Heartbeat_Callback);
}

void Overseer::Heartbeat_Callback()
{
  CommentStream->printf(
    "[%lu] Still alive.\n",
    millis()
  );

  PrintGyroHistory();
}

/*
  2023-10-07
  2023-10-08
  2023-10-14
  2023-10-17
  2023-10-21
  2023-11-03
  2023-11-07
  2023-11-11
  2023-11-13
*/
