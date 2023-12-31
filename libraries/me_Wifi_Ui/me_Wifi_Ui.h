#pragma once

#include <me_Types.h>
#include <me_Wifi.h>

namespace me_Wifi_Ui
{
  class TMeWifiUi
  {
    public:
      void PrintOurId();
      void PrintStations();
      TBool Connect();

    private:
      static const TUint_2 Message_MaxLength = 3 * 256;
      TChar Message [Message_MaxLength];

      void Stations_PrintHeader();
      void Stations_PrintFooter();
      void Stations_PrintRow(TUint_1 StationIndex, me_Wifi::TStation Station);

      void GetSecurityProtocolName(
        TChar* Message,
        TUint_2 Message_MaxLength,
        me_Wifi::TSecurityProtocol
      );
  };
}
