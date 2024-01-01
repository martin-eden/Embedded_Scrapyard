/*
  Status: stable
  Version: 1
  Last mod.: 2023-12-31
*/

#include "me_WifiShip_Ui.h"

#include <cstdio> // for snprintf()

using namespace me_WifiShip_Ui;

void me_WifiShip_Ui::RepresentCraftId(
  TChar* Message,
  TUint_2 Message_MaxLength,
  TCraftId CraftId
)
{
  snprintf(
    Message,
    Message_MaxLength,
    "%02X%02X%02X%02X%02X%02X",
    CraftId[0],
    CraftId[1],
    CraftId[2],
    CraftId[3],
    CraftId[4],
    CraftId[5]
  );
};

void me_WifiShip_Ui::RepresentCraftName(
  TChar* Message,
  TUint_2 Message_MaxLength,
  TCraftName CraftName
)
{
  snprintf(
    Message,
    Message_MaxLength,
    "%s",
    CraftName
  );
};


void me_WifiShip_Ui::RepresentCraftIds(
  TChar* Message,
  TUint_2 Message_MaxLength,
  TCraftIds CraftIds
)
{
  static const TChar MessageFmt[] PROGMEM =
    "+-------+\n"
    "| Craft |\n"
    "===================================================\n"
    "| Id           | Name                             |\n"
    "|--------------+----------------------------------|\n"
    "| %s | %-32s |\n"
    "===================================================\n"
    ;

  const TUint_1 CraftIdStr_Size = 1 * 16;
  TChar CraftIdStr[CraftIdStr_Size];

  const TUint_1 CraftNameStr_Size = 3 * 16;
  TChar CraftNameStr[CraftNameStr_Size];

  RepresentCraftId(CraftIdStr, CraftIdStr_Size, CraftIds.Id);
  RepresentCraftName(CraftNameStr, CraftNameStr_Size, CraftIds.Name);

  snprintf(
    Message,
    Message_MaxLength,
    MessageFmt,
    CraftIdStr,
    CraftNameStr
  );
}

/*
void TMeWifiUi::PrintStations()
{
  TUint_1 NumStationsFound;
  TBool ScanResult;

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

    Stations_PrintHeader();

    for (uint8_t StationIndex = 0; StationIndex < NumStationsFound; ++StationIndex)
    {
      using namespace me_Wifi;
      TStation StationInfo;

      GetStationInfo(StationIndex, &StationInfo);

      Stations_PrintRow(StationIndex, StationInfo);
    }

    Stations_PrintFooter();
  }
}

TBool TMeWifiUi::Connect()
{
  String StationIndexStr;
  const TChar Terminator = '\n';

  Serial.print("Enter the index (#) of station to connect: ");

  while (Serial.available() == 0) ;

  StationIndexStr = Serial.readStringUntil(Terminator);

  Serial.printf("StationIndex = \"%s\"\n", StationIndexStr.c_str());

  Serial.println();

  return true;
}

// -- private

void TMeWifiUi::Stations_PrintHeader()
{
  static const char Header[] PROGMEM =
    "+----------+\n"
    "| Stations |\n"
    "============================================================================================================\n"
    "|    |                      |                   |   |                    Channel                           |\n"
    "|    |                      |                   |   |------------------------------------------------------|\n"
    "|  # |        Name          |        MAC        | H | RSSI |  # |              Security                    |\n"
    "|----+----------------------+-------------------+---+------+----+------------------------------------------|\n"
    ;

  snprintf(Message, Message_MaxLength, Header);
  Serial.print(Message);
}

void TMeWifiUi::Stations_PrintFooter()
{
  static const char Footer[] PROGMEM =
    "============================================================================================================\n";

  snprintf(Message, Message_MaxLength, Footer);
  Serial.print(Message);
}

void TMeWifiUi::Stations_PrintRow(TUint_1 StationIndex, me_Wifi::TStation Station)
{
  static const char RowFmt[] PROGMEM =
    "| %2u | %-20s | %s | %s | %4d | %2u | %-40s |\n";

  const uint8_t SecurityName_MaxLength = 40;
  char SecurityProtocolName [SecurityName_MaxLength];

  GetSecurityProtocolName(
    SecurityProtocolName,
    SecurityName_MaxLength,
    Station.Channel.SecurityProtocol
  );

  snprintf(
    Message,
    Message_MaxLength,
    RowFmt,
    StationIndex,
    Station.Name,
    Station.Id,
    Station.IsHidden ? "X" : "-",
    Station.Channel.Strength,
    Station.Channel.Band,
    SecurityProtocolName
  );
  Serial.print(Message);
}
*/

/*
  Convert TSecurityProtocol enum value to string.
*/
/*
void TMeWifiUi::RepresentSecurityProtocol(
  TChar* Message,
  TUint_2 Message_MaxLength,
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
      snprintf(Message, Message_MaxLength, "(@ unknown security protocol value)");
      break;
  }
}
*/

/*
  2023-12-31 The only release in 2023.
*/
