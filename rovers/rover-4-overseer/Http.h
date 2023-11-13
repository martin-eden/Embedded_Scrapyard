// Http-end functions

/*
  Status: stable
  Version: 2
  Last mod.: 2023-11-13
*/

#pragma once

#include <Arduino.h>

#include <ESP8266WebServer.h>

namespace Http
{
  const uint16_t Response_Ok = 200;
  const uint16_t Response_NotFound = 404;
  const char Content_Plaintext[] = "text/plain";

  typedef std::function<void (void)> THandlerFunction;

  void Setup(THandlerFunction RootHandler_Callback);
  void HandleEvents();

  void SendString(String DataString);

  String GetClientIp();
}

/*
  2023-11-07
  2023-11-13
*/
