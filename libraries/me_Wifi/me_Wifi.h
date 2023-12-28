// Setting-up WiFi

/*
  Version: 4
  Last mod.: 2023-12-28
*/

#pragma once

#include <Arduino.h>

namespace me_Wifi
{
  constexpr auto *CommentStream = &Serial;

  /*
    Terminology.

    Under "station" I assume what is usually called "Access Point".
    Under "station IP" I assume what is called "DNS IP".

    I just imagining docking process in Elite: Odyssey where we are ship
    docking to other structure (Station).
  */

  class TNode_Settings
  {
    public:
      static const uint8_t Name_MaxLength = 33;
      static const uint8_t Id_MaxLength = 20;
  };

  struct TNode
  {
    char Name [TNode_Settings::Name_MaxLength];
    char Id [TNode_Settings::Id_MaxLength]; // MAC
  };

  enum struct TSecurityProtocol
  {
    None,
    Wep,
    Wpa,
    Wpa2,
    AnyWpa,
    Unknown
  };

  struct TChannel
  {
    uint8_t BandNumber;
    int8_t SignalStrength;
    TSecurityProtocol SecurityProtocol;
  };

  class TEstablishedChannel_Settings
  {
    public:
      static const uint8_t Address_MaxLength = 20;
  };

  struct TEstablishedChannel : TChannel
  {
    char StationAddress [TEstablishedChannel_Settings::Address_MaxLength]; // DNS
    char OurAddress [TEstablishedChannel_Settings::Address_MaxLength]; // IP
  };

  struct TStation : TNode
  {
    bool IsHidden;
  };

  /*
  class Docking
  {
    public:
      TNode Our;
      TChannel Connection;
      TStation Station;

      bool GetNumStationsNearby(uint8_t* NumStationsFound);

      bool GetStationInfo(
        uint8_t StationIndex,
        TStation* Station,
        TChannel* OfferedChannel
      );

      void ConnectTo(
        char const * StationName,
        char const * StationPassword,
        uint16_t Timeout_S = 32
      );

    private:
  }
  */

  // Initially we have name and id (MAC).
  String GetOurName();
  String GetOurId();

  // We can scan for nearby stations.
  bool Scan(uint8_t* NumStationsFound);

  // Get details about station.
  bool GetStationInfo(
    uint8_t StationIndex,
    TStation* Station,
    TChannel* Channel
  );

  // Then we are trying to dock with named station with password.
  bool ConnectTo(
    char const * StationName,
    char const * StationPassword,
    uint16_t Timeout_S = 32
  );

  // When we are connected, we have channel. Channel has strength (RSSI).
  int8_t GetDistance_Dbm();

  // Channel has two ends, station end and ship end.
  String GetOurAddress();
  String GetStationAddress();

  // After connection also we can peek at station id (MAC).
  String GetStationId();

}

/*
  2023-11-07
  2023-11-13
  2023-11-14
  2023-12-28 -- code structural design, network scan
*/
