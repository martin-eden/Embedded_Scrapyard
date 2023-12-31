// Setting-up WiFi

/*
  Version: 4
  Last mod.: 2023-12-31
*/

#pragma once

#include <Arduino.h>

#include <me_Types.h>

namespace me_Wifi
{
  constexpr auto *CommentStream = &Serial;

  /*
    Terminology.

    | Local   | Global
    +-----------------------------
    | Ship    | Access Point
    | Station | Router
    | Name    | Name
    | Id      | MAC
    | Address | IP

    I just imagining docking process in Elite: Odyssey where we are Ship
    docking to another structure (Station).
  */

  class TNode_Settings
  {
    public:
      static const TUint_1 Name_MaxLength = 33;
      static const TUint_1 Id_MaxLength = 20;
  };

  struct TNode
  {
    TChar Name [TNode_Settings::Name_MaxLength];
    TChar Id [TNode_Settings::Id_MaxLength]; // MAC
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
    TUint_1 BandNumber;
    TSint_1 SignalStrength;
    TSecurityProtocol SecurityProtocol;
  };

  class TEstablishedChannel_Settings
  {
    public:
      static const TUint_1 Address_MaxLength = 20;
  };

  struct TEstablishedChannel : TChannel
  {
    TChar StationAddress [TEstablishedChannel_Settings::Address_MaxLength]; // DNS
    TChar OurAddress [TEstablishedChannel_Settings::Address_MaxLength]; // IP
  };

  struct TStation : TNode
  {
    TBool IsHidden;
    TChannel Channel;
  };

  /*
  enum struct TDockingResult
  {
    Undocked,
    Docked,
    BadPassword,
    NoSuchStation,
    TimeoutReached,
    Unknown
  };

  class Docking
  {
    public:
      TNode Ship;
      TStation DockedStation;

      bool GetNumStationsNearby(uint8_t* NumStationsFound);

      bool GetStationInfo(uint8_t StationIndex, TStation* Station);

      TDockingResult DockTo(
        TUint_1 StationIndex,
        TChar const * StationPassword
      );

      TBool Undock();

    private:
      TUint_1 ConnectTimeout_S;
  }
  */

  // Initially we have name and id (MAC).
  String GetOurName();
  String GetOurId();

  // We can scan for nearby stations.
  TBool Scan(uint8_t* NumStationsFound);

  // Get details about station.
  TBool GetStationInfo(
    TUint_1 StationIndex,
    TStation* Station
  );

  // Then we are trying to dock with named station with password.
  TBool ConnectByName(
    TChar const * StationName,
    TChar const * StationPassword,
    TUint_1 Timeout_S = 32
  );

  // When we are connected, we have channel. Channel has strength (RSSI).
  TSint_1 GetDistance_Dbm();

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
  2023-12-31 -- using <me_Types.h>
*/
