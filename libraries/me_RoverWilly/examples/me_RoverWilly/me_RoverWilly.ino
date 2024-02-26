// Firmware for Willy rover

/*
  Version: 11
  Last mod.: 2024-02-26
*/

/*
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
    HeartbeatInterval_S = 60,
    TickTime_Ms = 50;

  constexpr auto *CommentStream = &Serial;

  Motorboard::MotorboardChannel MotorboardStream;

  bool MotorboardIsConnected;
  bool GyroIsConnected;
  bool WifiIsConnected;

  Ticker GyroPoll_Timer;
  Ticker Heartbeat_Timer;

  void PrintLabel();
  void PrintSettings();

  void Http_Root_Callback();
  void Http_SendCommand_Callback();

  void StartGyroPolling(uint16_t Interval_Ms);
  void StopGyroPolling();
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

  MotorboardIsConnected =
    MotorboardStream.Setup(
      Motorboard_Baud,
      Motorboard_Receive_Pin,
      Motorboard_Transmit_Pin
    );

  if (MotorboardIsConnected)
  {
    CommentStream->print("Measuring motorboard ping: ");
    uint16_t PingValue_Ms = Motorboard::MeasurePing_Ms(MotorboardStream);
    CommentStream->printf("%d ms\n", PingValue_Ms);
  }

  GyroIsConnected = SetupGyro();

  me_Wifi::Init();
  uint16_t WifiConnectTimeout_S = 20;
  WifiIsConnected =
    me_Wifi::SetupWifi(
      StationName,
      StationPassword,
      WifiConnectTimeout_S
    );

  if (WifiIsConnected)
  {
    Http::Setup();
    Http::AddEndpoint("/", Http_Root_Callback);
    Http::AddEndpoint("/SendCommand/", Http_SendCommand_Callback);
  }

  SetupHeartbeat(HeartbeatInterval_S);

  CommentStream->printf(
    PSTR(
      "Modules\n"
      "\n"
      "  Motorboard = %u\n"
      "  Gyro = %u\n"
      "  WiFi = %u\n"
      "\n"
    ),
    MotorboardIsConnected,
    GyroIsConnected,
    WifiIsConnected
  );

  CommentStream->printf(
    "--] Setup\n"
  );

  if (MotorboardIsConnected)
  {
    // WROOM-WROOM!
    Motorboard::RunMotorsTest(MotorboardStream);
  }
}

void loop()
{
  if (WifiIsConnected)
  {
    Http::HandleEvents();
  }

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
    ),
    Serial_Baud,
    Motorboard_Baud,
    Motorboard_Receive_Pin,
    Motorboard_Transmit_Pin
  );
}

void Overseer::Http_Root_Callback()
{
  static const char
    CommandPoint_Html[] PROGMEM =
      "<!DOCTYPE html>\n"
      "\n"
      "<html>\n"
      "  <head>\n"
      "    <title> Rover-4 Post Office </title>\n"
      "    <style>\n"
      "      body {font-family: Roboto Mono Medium; color: rgb(166, 226, 44); background-color: rgb(40, 41, 35)}\n"
      "      input[type=text] {font-family: Roboto Mono Medium; color: rgb(248, 246, 226); background-color: rgb(40, 41, 35)}\n"
      "      input[type=submit] {font-family: Roboto Mono Medium; color: rgb(103, 216, 239); background-color: rgb(40, 41, 35)}\n"
      "      form {position: fixed; left: 40%; top: 40%}\n"
      "    </style>\n"
      "  </head>\n"
      "  <body>\n"
      "    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/SendCommand/\">\n"
      "      <input type=\"text\" id=\"fCommand\" name=\"Command\" value=\"L 100 R 100 D 500 \">\n"
      "      <label for=\"fCommands\">&lt;- Command -&gt;</label>\n"
      "      <input type=\"submit\" value=\"Send\">\n"
      "    </form>\n"
      "  </body>\n"
      "</html>\n"
      "\n";

  Http::SendHtml(CommandPoint_Html);

  CommentStream->printf(
    "[%lu] Sent main page to %s.\n",
    millis(),
    Http::GetClientIp().c_str()
  );
}

void Overseer::Http_SendCommand_Callback()
{
  String FinalMessage;

  uint8_t MaxMessageSize = 200;
  char Message[MaxMessageSize];

  String Command = Http::Request_GetEntityValueByName("Command");

  StartGyroPolling(20);

  uint32_t CommandStartTime_Ms = millis();

  MotorboardStream.Send(Command.c_str());

  uint32_t CommandEndTime_Ms = millis();

  StopGyroPolling();

  snprintf(
    Message,
    MaxMessageSize,
    "[%u..%u] \"%s\"\n",
    CommandStartTime_Ms,
    CommandEndTime_Ms,
    Command.c_str()
  );
  FinalMessage.concat(Message);

  FinalMessage.concat(GetGyroHistory_Json());

  CommentStream->print(FinalMessage);
  Http::SendPlaintext(FinalMessage);
}

void Overseer::StartGyroPolling(uint16_t Interval_Ms)
{
  GyroPoll_Timer.attach_ms(Interval_Ms, GyroPoll_IsrCallback);
}

void Overseer::StopGyroPolling()
{
  GyroPoll_Timer.detach();
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
  2023-11-15
  2024-02-26
*/
