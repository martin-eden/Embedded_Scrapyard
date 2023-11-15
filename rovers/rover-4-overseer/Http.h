// Http-end functions

/*
  Status: stable
  Version: 3
  Last mod.: 2023-11-15
*/

#pragma once

#include <Arduino.h>

#include <ESP8266WebServer.h>

namespace Http
{
  typedef std::function<void (void)> THandlerFunction;

  void Setup(THandlerFunction RootHandler_Callback);
  void HandleEvents();

  void SendPlaintext(String DataString);
  void SendHtml(String DataString);

  String GetClientIp();
}

/*
  2023-11-07
  2023-11-13
  2023-11-15
*/
