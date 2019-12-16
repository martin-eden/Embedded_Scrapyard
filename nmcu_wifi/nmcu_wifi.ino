#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h> // alias for `#include “SSD1306Wire.h”`
SSD1306  display(0x3c, D2, D1);

const char* ssid = “your_local_ssid”;
const char* password = “your_wifi_password”;
const char* host = “ESP-OTA”;

#define led_pin BUILTIN_LED
#define blu_pin D4
#define beat 450
IPAddress ip(192, 168, 1, 196); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
void setup() {
  Serial.begin(57600);

  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  delay(25);

  pinMode(led_pin, OUTPUT);

  Serial.println(“”);
  Serial.println(“Booting”);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println(“Retrying connection…”);
    delay(5);
  }
  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
    analogWrite(led_pin, 0);
  });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
    for (int i = 0; i < 80; i++)
    {
      digitalWrite(led_pin, HIGH);
      delay(i * 2);
      digitalWrite(led_pin, LOW);
      delay(i * 2);
    }
    ESP.restart();
  });

  ArduinoOTA.onError([](ota_error_t error) {
    ESP.restart();
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println(“Ready”);
  Serial.print(“IP address: “);
  Serial.println(WiFi.localIP());
  Serial.println(“Now Online!”);
}

void loop() {
  // clear the display
  display.clear();
  float power = WiFi.RSSI();
 String strpower = String(power, 1);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 18, ssid);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 32, “POWER: ” + strpower);
  display.display();
  analogWrite(blu_pin, power);
  ArduinoOTA.handle();
  heartbeat();
  delay(30);
}

void heartbeat() {
  digitalWrite(led_pin, HIGH);
  delay(beat);
  digitalWrite(led_pin, LOW);
  delay(beat);
}