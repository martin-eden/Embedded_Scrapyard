// Http-end functions

/*
  Status: stable
  Version: 1
  Last mod.: 2023-11-09
*/

#include "Http.h"

using namespace Http;

ESP8266WebServer HttpServer;

// ---

void Http_HandleRoot();
void Http_HandleNotFound();

void Http::Setup(THandlerFunction RootCallback_Func)
{
  Serial.println("Setting-up HTTP: [");

  HttpServer.on("/", RootCallback_Func);
  Serial.println("  Added / hook.");

  HttpServer.onNotFound(Http_HandleNotFound);
  Serial.println("  Added NOT_FOUND hook.");

  HttpServer.begin();

  Serial.println("]");
}

void Http::HandleEvents()
{
  HttpServer.handleClient();
}

void Http::SendString(String s)
{
  HttpServer.send(Http_Response_Ok, Http_Content_Plaintext, s);
}

String Http::GetClientIp()
{
  return HttpServer.client().remoteIP().toString();
}

// ---

void Http_HandleNotFound()
{
  HttpServer.send(Http_Response_NotFound, Http_Content_Plaintext, "Not Found\n\n");
}

// ---

/*
  2023-11-07
  2023-11-09
*/
