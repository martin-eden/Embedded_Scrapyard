#pragma once

#include <Arduino.h>

#include <ESP8266WebServer.h>

const uint16_t Http_Response_Ok = 200;
const uint16_t Http_Response_NotFound = 404;
const char Http_Content_Plaintext[] = "text/plain";

typedef std::function<void (void)> THandlerFunction;

void SetupHttp(THandlerFunction RootCallback_Func);
void HandleHttp();
void SendStrHttp(String s);
String GetHttpClientIp();
