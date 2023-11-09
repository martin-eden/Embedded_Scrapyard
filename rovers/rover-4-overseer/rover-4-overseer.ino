// Firmware for my Rover v4.

/*
  Status: working base
  Version: 8
  Last mod.: 2023-11-09
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

#include <Ticker.h>

#include "Motors.h"
#include "Gyro.h"
#include "GyroHistory.h"

#include "WiFi.h"
#include "Http.h"

const char
  * StationName = "",
  * StationPassword = "";

const uint32_t
  Serial_Baud = 115200,
  Motorboard_Baud = 9600;

const uint8_t
  Motorboard_Receive_Pin = D7,
  Motorboard_Transmit_Pin = D9;

const uint32_t
  GyroPollInterval_Ms = 1200,
  HeartbeatInterval_Ms = 20000,
  TickTime_Ms = 50;

Ticker Timer;

void SetupSerial();
void PrintBanner();
void SendGyroReadings_Callback();

// ---

void setup()
{
  Serial.begin(Serial_Baud);

  PrintBanner();

  // SetupWiFi(StationName, StationPassword);

  // Http::Setup(SendGyroReadings_Callback);

  SetupGyro();

  SetupIsr();

  bool MotorboardIsConnected =
    SetupMotorboardCommunication(
      Motorboard_Baud,
      Motorboard_Receive_Pin,
      Motorboard_Transmit_Pin
    );

  if (MotorboardIsConnected)
  {
    HardwareMotorsTest();
  }
}

void Heartbeat();

void loop()
{
  // Heartbeat();

  Http::HandleEvents();

  delay(TickTime_Ms);
}

// ---

void PrintBanner()
{
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println(" Rover-4 overseer with accelerometer ");
  Serial.println("-------------------------------------");
}

void SendGyroReadings_Callback()
{
  String GyroReadings_Str = SerializeGyroReadings(GetLastGyroReadings(), GetLastGyroReadingsTime_Ms());

  Http::SendString(GyroReadings_Str);

  Serial.printf("[%lu] Sent gyro readings to %s.\n", millis(), Http::GetClientIp().c_str());
}

void GyroPoll_Isr();

void SetupIsr()
{
  Timer.attach_ms(GyroPollInterval_Ms, GyroPoll_Isr);

  Serial.printf("Set-up gyro polling every %d ms.\n", GyroPollInterval_Ms);
}

void IRAM_ATTR GyroPoll_Isr()
{
  UpdateGyroReadings();
  StoreGyroReadings(GetLastGyroReadings(), GetLastGyroReadingsTime_Ms());
}

void Heartbeat()
{
  static uint32_t LastPrintTime_Ms = millis();
  uint32_t CurrentTime_Ms = millis();
  uint32_t TimePassed_Ms = CurrentTime_Ms - LastPrintTime_Ms;
  if (TimePassed_Ms > HeartbeatInterval_Ms)
  {
    LastPrintTime_Ms = CurrentTime_Ms;

    Serial.printf("[%lu] Still alive.\n", millis());

    PrintGyroHistory();
  }
}

/*
  2023-10-07
  2023-10-08
  2023-10-14
  2023-10-17
  2023-10-21
  2023-11-03
  2023-11-07
*/
