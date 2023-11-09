// Http-end functions

/*
  Status: stable
  Version: 1
  Last mod.: 2023-11-09
*/

#pragma once

#include <Arduino.h>

#include <ESP8266WebServer.h>

namespace Http
{
  const uint16_t Http_Response_Ok = 200;
  const uint16_t Http_Response_NotFound = 404;
  const char Http_Content_Plaintext[] = "text/plain";

  typedef std::function<void (void)> THandlerFunction;

  void Setup(THandlerFunction RootCallback_Func);
  void HandleEvents();
  void SendString(String s);
  String GetClientIp();
}
