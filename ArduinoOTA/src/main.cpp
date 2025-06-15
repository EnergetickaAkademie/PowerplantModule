#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ota.h>
#include "secrets.h"

#define BLINK_INTERVAL 500

void setup() {
	Serial.begin(115200);
	Serial.println("Booting");
	
	connectWifi(SECRET_SSID, SECRET_PASSWORD);
	
	setupOTA();

	Serial.println("Ready");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	pinMode(LED_BUILTIN, OUTPUT);
}

void blinkLED() {
	digitalWrite(LED_BUILTIN, HIGH);
	delay(BLINK_INTERVAL);
	digitalWrite(LED_BUILTIN, LOW);
	delay(BLINK_INTERVAL);
}

void loop() {
	handleOTA();

	blinkLED();
}