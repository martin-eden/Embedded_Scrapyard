// Connect to WiFi router

/*
  Status: redesigning
  Version: 5
  Last mod.: 2023-12-31
*/

/*
  System library interface is usually ugly.

  System library needs to support backward compatibility and
  provide access to new features. So it's names and structures
  can remain at 1980 level or they can represent a weird mixture
  of design solutions.

  I do not want that ugliness in my code.

  Ugliness stops here. It's a wrapper over <ESP8266WiFi.h>.
  It does what I need and nothing else. Structures and classes
  named as I wanted. Connection modes that I don't need are
  not supported.

  What is supported?

  Network scan to get list of nearby routers. Connection to router
  by MAC. Retrieving IP address. Disconnection from router.

  Cheers, Ivan Grokhotkov!
*/

#pragma once

// #include <me_WifiShip_Scanner.h>
// #include <me_WifiShip_Docker.h>

#include <me_Types.h>

/*
  Interface terminology

    I just imagining docking process in Elite: Odyssey where we are Ship
    docking to another structure (Station). So for memorable interface
    I'm using these terms:

    | Local   | Common
    +-----------------------------
    | Ship    | Access Point
    | Station | Router
    | Id      | MAC
    | Name    | Name
    | Address | IP

  Usage

    * You can connect to station if you know it's name and password:

        DockingResult = Ship.Docker.DockTo(StationName, StationPassword)
        if (DockingResult == Docked)
          print ("IP = $Ship.Docker.Connection.ShipAddress")

    * You can scan for nearby stations to get their names:

        ScanWentFine = Ship.Scanner.Scan(&NumStations);
        if (ScanWentFine)
          for StationIndex = 0, NumStations - 1
            GotStationInfo = Ship.Scanner.GetStationInfo(StationIndex, &StationInfo)
            if (GotStationInfo)
              print ("Name = $StationInfo.Name, ...")

  Implementation terminology

    For implementation details I'm using these terms.

      Craft - any device in WiFi network with SSID and BSSID

        Name - name of device as it is seen in network. This is what
          practically needed for connection.

        Id - MAC address of device. That is what theoretically needed
          for connection.

          Internal Esplora API requires name for connection. So you
          can't even try to connect to hidden network as it's name is
          empty.

      Station(extends Craft) - router that we can try to connect

        IsHidden - has no name, usually not something that welcomes connects

        Channel - offered frequency band. See Channel.

      Channel - radio band channel that is used for data transmission

        Band - number of channel. Physically it describes range of radio
          frequencies (band).

        Strength - received signal strength indicator (RSSI).

          Weird negative integer number that represents logarithm
          of signal-to-noise-ratio.

          Maybe I'll to convert internal representation to something
          more useful.

        SecurityProtocol - WEP, WPA 1, WPA 2 or (WPA 1 + WPA 2).

            WEP - cracked
            WPA 1 - deprecated
            WPA 1 + WPA 2 - deprecated support
            WPA 2 - normal router should use this

        # Actually we can retrieve more channel details but I don't
        # need them for my projects.

      Connection(extends Channel) - we are connected and got IP

        StationAddress - DNS IP. Something like 192.168.0.1.

        ShipAddress - IP. Our IP in local network. Something like
          192.168.0.208.
*/

namespace me_WifiShip
{
  const TUint_1 TCraftId_Size = 6;
  typedef TUint_1 TCraftId[TCraftId_Size];

  const TUint_1 TCraftName_Size = 32 + 1;
  typedef TChar TCraftName[TCraftName_Size];

  struct TCraftIds
  {
    TCraftId Id; // MAC
    TCraftName Name;
  };

  class TWifiShip
  {
    public:
      // TWifiShip_Scanner Scanner;
      // TWifiShip_Docker Docker;

      void Init();

      TBool GetShipId(TCraftId* ShipId);
      TBool SetShipId(TCraftId ShipId);

      TBool GetShipName(TCraftName* ShipName);
      TBool SetShipName(TCraftName ShipName);

      TBool GetShipIds(TCraftIds* ShipIds);
      TBool SetShipIds(TCraftIds ShipIds);
  };
}

/*
  2023-11-07
  2023-11-13
  2023-11-14
  2023-12-28 -- code structural design, network scan
  2023-12-31 -- using <me_Types.h>
  2024-01-01 -- splat to three modules: ship = (frame, scanner, docker)
*/
