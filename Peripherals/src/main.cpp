#include <Arduino.h>
#include "PeripheralFactory.h"

#define LATCH_PIN D1
#define DATA_PIN  D2
#define CLOCK_PIN D0

PeripheralFactory factory;
ShiftRegisterChain* shiftChain = factory.createShiftRegisterChain(LATCH_PIN, DATA_PIN, CLOCK_PIN);

Bargraph* bargraph2 = factory.createBargraph(shiftChain, 10);
SegmentDisplay* display2 = factory.createSegmentDisplay(shiftChain, 4);
Bargraph* bargraph1 = factory.createBargraph(shiftChain, 10);
SegmentDisplay* display1 = factory.createSegmentDisplay(shiftChain, 4);

bool reversed;
void setup() {
	Serial.begin(115200);
	
	factory.init();

	reversed = false;
}


void loop() {
	factory.update();
	
	if(millis() % 100 == 0) {
		float timesec = (float)millis()/1000;

		display1->displayNumber(timesec, 1);
		display2->displayNumber((float)1000 - timesec, 1);
	}
	
	if(millis() % 1000 == 0) {

		bargraph1->setValue((millis() / 1000) % 11);
		bargraph2->setValue((millis() / 1000) % 11);

		if ((millis() / 1000) % 11 == 0) {
			reversed = !reversed;
			bargraph1->setReversed(reversed);
			bargraph2->setReversed(!reversed);
			Serial.println(reversed ? F("Bargraph reversed") : F("Bargraph normal"));
		}
	}

}