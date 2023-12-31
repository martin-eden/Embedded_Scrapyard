// My WiFi utility library test/exploration.

/*
  Status: good base
  Version: 2
  Last mod.: 2023-12-31
*/

/*
  Board: Esplora (Wemos D1)
*/

#include <me_Wifi.h> // remove(!)

#include <me_Wifi_Ui.h>
#include <me_Types.h>

const TUint_4
  SerialSpeed = 115200;

const TUint_4
  RescanInterval_S = 1.5 * 60;

const TUint_2 Message_MaxLength = 1 * 256;
TChar Message [Message_MaxLength];

me_Wifi_Ui::TMeWifiUi WifiUi;

void PrintGreeting()
{
  Serial.println("Demo of <me_Wifi_Ui.h> which uses <me_Wifi.h>.");
  Serial.println();
}

void PrintSettings()
{
  snprintf(
    Message,
    Message_MaxLength,
    "Rescan interval (s): %lu",
    RescanInterval_S
  );
  Serial.println(Message);

  WifiUi.PrintOurId();
}

void setup()
{
  const TUint_2 SerialSetupDelay_Ms = 500;

  Serial.begin(SerialSpeed);

  delay(SerialSetupDelay_Ms);

  Serial.println();
  Serial.println();

  PrintGreeting();
  PrintSettings();
}

void loop()
{
  static TBool IsConnected = false;

  WifiUi.PrintStations();

  IsConnected = WifiUi.Connect();

  if (IsConnected)
  {
    Serial.println("Connected to station.");
  }

  delay(RescanInterval_S * 1000);
}

/*
  2023-12-26
  2023-12-28
  2023-12-30
  2023-12-31
*/
