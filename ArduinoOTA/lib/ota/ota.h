#ifndef OTA_H
#define OTA_H

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


void connectWifi(const char* ssid, const char* password);
void setupOTA(int port = -1, const char* hostname = nullptr, const char* password = nullptr);
void handleOTA();


#endif // OTA_H