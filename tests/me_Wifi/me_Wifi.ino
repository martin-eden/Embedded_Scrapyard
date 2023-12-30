// My WiFi utility library test/exploration.

/*
  Status: good base
  Version: 1
  Last mod.: 2023-12-30
*/

/*
  Board: Esplora (Wemos D1)
*/

#include <me_Wifi.h>

const uint32_t
  SerialSpeed = 115200;

const uint32_t
  ScanInterval_S = 1.5 * 60;

const uint16_t Message_MaxLength = 3 * 256;
char Message[Message_MaxLength];

void setup()
{
  Serial.begin(SerialSpeed);

  delay(500);
  Serial.println();
  Serial.println();

  PrintGreeting();
}

void loop()
{
  PrintStatus();
  DoScan();
  delay(ScanInterval_S * 1000);
}

void PrintGreeting()
{
  Serial.println("Demo <me_Wifi.h>.");
  Serial.println();
}

void PrintStatus()
{
  static const char MessageFmt[] PROGMEM =
    "+====+\n"
    "| We |\n"
    "+==========================================+\n"
    "|          Name        |        MAC        |\n"
    "|----------------------+-------------------|\n"
    "| %-20s | %s |\n"
    "+==========================================+\n"
    ;

  {
    using namespace me_Wifi;

    snprintf(
      Message,
      Message_MaxLength,
      MessageFmt,
      GetOurName().c_str(),
      GetOurId().c_str()
    );
    Serial.println(Message);
  }

  snprintf(
    Message,
    Message_MaxLength,
    "Rescan interval (s): %u",
    ScanInterval_S
  );
  Serial.println(Message);

  Serial.println();
}

void DoScan()
{
  uint8_t NumStationsFound;
  bool ScanResult;

  Serial.printf("Doing scan... ");

  {
    using namespace me_Wifi;

    ScanResult = Scan(&NumStationsFound);
  }

  if (ScanResult)
    Serial.println("yep.");
  else
    Serial.println("nah!");

  if (ScanResult)
  {
    Serial.println();
    snprintf(Message, Message_MaxLength, "%u stations found.", NumStationsFound);
    Serial.println(Message);
    Serial.println();

    PrintHeader();

    for (uint8_t StationIndex = 0; StationIndex < NumStationsFound; ++StationIndex)
    {
      using namespace me_Wifi;
      TStation StationInfo;

      GetStationInfo(StationIndex, &StationInfo);

      PrintRow(StationInfo);
    }

    PrintFooter();
  }
}

void PrintHeader()
{
  static const char Header[] PROGMEM =
    "+==========+\n"
    "| Stations |\n"
    "+=====================================================================================================+\n"
    "|                      |                   |   |                    Channel                           |\n"
    "|                      |                   |   |------------------------------------------------------|\n"
    "|        Name          |        MAC        | H | RSSI |  # |              Security                    |\n"
    "|----------------------+-------------------+---+------+----+------------------------------------------|\n"
    ;

  snprintf(Message, Message_MaxLength, Header);
  Serial.print(Message);
}

void PrintFooter()
{
  static const char Footer[] PROGMEM =
    "+=====================================================================================================+\n";

  snprintf(Message, Message_MaxLength, Footer);
  Serial.print(Message);
}

void PrintRow(me_Wifi::TStation Station)
{
  static const char RowFmt[] PROGMEM =
    "| %-20s | %s | %s | %4d | %2u | %-40s |\n";

  const uint8_t SecurityName_MaxLength = 40;
  char SecurityProtocolName [SecurityName_MaxLength];

  GetSecurityName(
    SecurityProtocolName,
    SecurityName_MaxLength,
    Station.Channel.SecurityProtocol
  );

  snprintf(
    Message,
    Message_MaxLength,
    RowFmt,
    Station.Name,
    Station.Id,
    Station.IsHidden ? "X" : "-",
    Station.Channel.SignalStrength,
    Station.Channel.BandNumber,
    SecurityProtocolName
  );
  Serial.print(Message);
}

bool GetSecurityName(
  char* Message,
  uint16_t Message_MaxLength,
  me_Wifi::TSecurityProtocol SecurityProtocol
)
{
  using namespace me_Wifi;

  switch (SecurityProtocol)
  {
    case TSecurityProtocol::None:
      snprintf(Message, Message_MaxLength, "None");
      break;

    case TSecurityProtocol::Wep:
      snprintf(Message, Message_MaxLength, "WEP");
      break;

    case TSecurityProtocol::Wpa:
      snprintf(Message, Message_MaxLength, "WPA v1");
      break;

    case TSecurityProtocol::Wpa2:
      snprintf(Message, Message_MaxLength, "WPA v2");
      break;

    case TSecurityProtocol::AnyWpa:
      snprintf(Message, Message_MaxLength, "WPA v1 + WPA v2");
      break;

    case TSecurityProtocol::Unknown:
      snprintf(Message, Message_MaxLength, "(we failed to detect security protocol)");
      break;

    default:
      Serial.printf("@ Unknown security protocol value.");
      return false;
  }

  return true;
}

/*
  2023-12-26
  2023-12-28
  2023-12-30
*/
