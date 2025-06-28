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

//devices are addd in the reverse order, due to the chaining of the shift registers
Bargraph* bargraph1 = factory.createBargraph(shiftChain, 10);
SegmentDisplay* display1 = factory.createSegmentDisplay(shiftChain, 8);

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
	
	
	if(millis() % 100 == 0) {
		display1->displayNumber(millis());
	}
	
	if(millis() % 1000 == 0) { // Update every second
		//display1->displayString("12345678");
		static bool reversed = false;

		bargraph1->setValue((millis() / 1000) % 11); // Example: Update bargraph based on millis

		//reverse the order after every 10 seconds
		if ((millis() / 1000) % 11 == 0) {
			reversed = !reversed;
			bargraph1->setReversed(reversed);
			Serial.println(reversed ? F("Bargraph reversed") : F("Bargraph normal"));
		}

		// create the raw data for testing (two bytes), do not use a for loop
		// byte* raw = new byte[bargraph1->getRegisterCount()];
		
		// //create some dummy data without a for loop
		// raw[0] = 0b00000011;
		// raw[1] = 0b11111111;
		
		// bargraph1->setRawData(raw, 2);

		const byte* data = bargraph1->getShiftData();
		Serial.print(F("Bargraph Data: "));
		for (int i = 0; i < bargraph1->getRegisterCount(); i++) {
			for (int j = 0; j < 8; j++) {
				Serial.print((data[i] & (1 << j)) ? '1' : '0');
			}
			Serial.print(' ');
		}
		Serial.println();
	}
}