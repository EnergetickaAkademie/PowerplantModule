#include "ota.h"

void connectWifi(const char* ssid, const char* password){
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}
}

void setupOTA(int port, const char* hostname, const char* password) {
	// Port defaults to 8266

	if (port > 0){
		ArduinoOTA.setPort(port);
	}

	if (hostname != nullptr) {
		ArduinoOTA.setHostname(hostname);
	}

	if (password != nullptr) {
		ArduinoOTA.setPassword(password);
	}

	ArduinoOTA.onStart([]() {
		Serial.println("Start");
	});
	
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});

	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});

	ArduinoOTA.begin();
}

void handleOTA(){
	ArduinoOTA.handle();
}