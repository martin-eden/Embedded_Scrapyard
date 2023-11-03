#include "Http.h"

ESP8266WebServer Http;

void Http_HandleRoot();
void Http_HandleNotFound();

void SetupHttp(THandlerFunction RootCallback_Func)
{
  Serial.println("Setting-up HTTP: [");

  Http.on("/", RootCallback_Func);
  Serial.println("  Added / hook.");

  Http.onNotFound(Http_HandleNotFound);
  Serial.println("  Added NOT_FOUND hook.");

  Http.begin();

  Serial.println("]");
}

void Http_HandleNotFound()
{
  Http.send(Http_Response_NotFound, Http_Content_Plaintext, "Not Found\n\n");
}

void HandleHttp()
{
  Http.handleClient();
}

void SendStrHttp(String s)
{
  Http.send(Http_Response_Ok, Http_Content_Plaintext, s);
}

String GetHttpClientIp()
{
  return Http.client().remoteIP().toString();
}
