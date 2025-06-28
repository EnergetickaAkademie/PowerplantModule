#include <Arduino.h>
#include "wemos_pins.h"
#include "PeripheralFactory.h" // Use the factory again
#include "ota.h"
#include "secrets.h"
#include <Wire.h>

PeripheralFactory factory;
LiquidCrystal* lcd = nullptr;

// Timers for non-blocking updates
unsigned long lastInfoUpdateTime = 0;
unsigned long lastScrollTime = 0;

void setup(){
	Serial.begin(115200);
	Wire.begin();

	lcd = factory.createLiquidCrystal(0x27, 20, 4);

	connectWifi(SECRET_SSID, SECRET_PASSWORD);
	setupOTA();

	factory.init();

	if (lcd) {
		lcd->setCursor(11, 0);
		lcd->print(" <-> ESP8266 Powerplant Module                 ");
	}

	Serial.println(F("Ready"));
	Serial.print(F("IP address: "));
	Serial.println(WiFi.localIP());
}


void loop() {
	handleOTA();
	factory.update();

	if (millis() % 250 == 0) {
		if (lcd) {
			lcd->scrollDisplayLeft();
		}
	}

	if (millis() % 1000 == 0) {
		if (lcd) {
			lcd->setCursor(0, 1);
			lcd->print("IP: ");
			lcd->print(WiFi.localIP().toString().c_str());
			lcd->print("          ");

			lcd->setCursor(0, 2);
			lcd->print("Time: ");
			lcd->print(millis() / 1000);
			lcd->print("s ");
			lcd->print("          ");
		}
	}
}