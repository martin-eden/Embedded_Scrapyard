// WifiShip: Structured approach to base WiFi functionality.

/*
  Status: nice
  Version: 5
  Last mod.: 2024-02-20
*/

/*
  System library interface is usually ugly.

  System library needs to support backward compatibility and provide
  access to new features. So it's names and structures can remain flat
  and at 1980 level or they can represent amazing mixture of design
  solutions.

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

/*
  Interface terminology

    I love to play games. And when we are talking about WiFi scanning
    and connecting, I'm just imagining docking process in Elite:
    Odyssey where we are Ship which is docking to Station. So for
    memorable interface I'm using these terms:

    | Local   | Common
    +-----------------------------
    | Ship    | Access Point
    | Station | Router
    | Id      | MAC
    | Name    | Name
    | Address | IP

  Design

    This ship has three parts:

      * Core

        Ship core is it's Name and Id (SSID and MAC). You can change them.

      * Scanner

        Scanner scans for nearby stations. Call Scan() to retrieve list of stations.

      * Docker

        Docker docks/undocks to station: DockTo(Name, Password).
        When docked you have two-ended Channel with ShipAddress and
        StationAddress (local IP and DNS IP).

  Classes

    WifiShip: TWifiShip
      Core: TCore
      Scanner: TScanner
      Docker: TDocker

      Init(): bool

  Source files

    * Parts

      Core - <Core/Interface.h>
      Scanner - <Scanner/Interface.h>
      Docker - <Docker/Interface.h>

    * UI Modules

      Core - <Core/Ui/Interface.h>
      Scanner - <Scanner/Ui/Interface.h>
      Docker - <Docker/Ui/Interface.h>

      Routines there provide text representation of relative data
      structures. They have common design of snprintf():

        void (TChar* Message, TUint_2 Message_MaxLength, ...)

    * Demo/test

      <me_WifiShip.ino>

      Uses WifiShip and it's UI modules.
*/

#include "Core/Interface.h"
#include "Scanner/Interface.h"
#include "Docker/Interface.h"

namespace me_WifiShip
{
  class TWifiShip
  {
    public:
      me_WifiShip_Core::TCore Core;
      me_WifiShip_Docker::TDocker Docker;
      me_WifiShip_Scanner::TScanner Scanner;

      TBool Init();
  };
}

/*
  2023-11-07
  2023-11-13
  2023-11-14
  2023-12-28 -- code structural design, network scan
  2023-12-31 -- using <me_Types.h>
  2024-01-01 -- splat to three modules: ship = (frame, scanner, docker)
  2024-01-03
  2024-02-20 -- standalone repo on GitHub
*/
