#include <Arduino.h>
#include "wemos_pins.h"
#include "PeripheralFactory.h"
#include "ota.h"
#include "secrets.h"

#define LATCH_PIN D1
#define DATA_PIN  D2
#define CLOCK_PIN D0
#define NUM_DIGITS 8

PeripheralFactory factory;
ShiftRegisterChain* shiftChain = factory.createShiftRegisterChain(LATCH_PIN, DATA_PIN, CLOCK_PIN);

SegmentDisplay* display1 = factory.createSegmentDisplay(shiftChain, 8);
//Bargraph* bargraph1 = factory.createBargraph(shiftChain, 8);
//SegmentDisplay* display2 = factory.createSegmentDisplay(shiftChain, 4);

unsigned long lastUpdateTime = 0;

void setup() {
	Serial.begin(115200);
	
	//connectWifi(SECRET_SSID, SECRET_PASSWORD);
	//setupOTA();
	
	factory.init();
	
	Serial.println(F("Ready"));
	Serial.print(F("IP address: "));
	Serial.println(WiFi.localIP());
}

void loop() {
	//handleOTA();
	factory.update();

	display1->displayNumber(millis());
	//display1->displayString("12345678");
}