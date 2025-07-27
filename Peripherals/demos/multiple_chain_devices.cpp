#include <Arduino.h>
#include "PeripheralFactory.h"

#define LATCH_PIN D1
#define DATA_PIN  D2
#define CLOCK_PIN D0

PeripheralFactory factory;

/* DEVICES */
ShiftRegisterChain* shiftChain = nullptr;

/* SHIFT CHAIN DEVICES */
SegmentDisplay* ldisplay2 = nullptr;
SegmentDisplay* ldisplay1 = nullptr;
Bargraph* bargraph7 = nullptr;
SegmentDisplay* display7 = nullptr;
Bargraph* bargraph6 = nullptr;
SegmentDisplay* display6 = nullptr;
Bargraph* bargraph5 = nullptr;
SegmentDisplay* display5 = nullptr;
Bargraph* bargraph4 = nullptr;
SegmentDisplay* display4 = nullptr;
Bargraph* bargraph3 = nullptr;
SegmentDisplay* display3 = nullptr;
Bargraph* bargraph2 = nullptr;
SegmentDisplay* display2 = nullptr;
Bargraph* bargraph1 = nullptr;
SegmentDisplay* display1 = nullptr;

bool reversed;

void updateBargraphs() {
	bargraph1->setValue((millis() / 1000) % 11);
	bargraph2->setValue((millis() / 900) % 11);
	bargraph3->setValue((millis() / 800) % 11);
	bargraph4->setValue((millis() / 700) % 11);
	bargraph5->setValue((millis() / 600) % 11);
	bargraph6->setValue((millis() / 500) % 11);
	bargraph7->setValue((millis() / 400) % 11);
}

void toggleReverse() {
	reversed = !reversed;
	bargraph1->setReversed(reversed);
	bargraph2->setReversed(!reversed);
	Serial.println(reversed ? F("Bargraph reversed") : F("Bargraph normal"));
}

void updateDisplays() {
	float timesec = (float)millis()/1000;
	display1->displayNumber(timesec, 1);
	display2->displayNumber((float)1000 - timesec, 1);
	display3->displayNumber((float)timesec * 2, 1);
	display4->displayNumber((float)timesec * 3, 1);
	display5->displayNumber((float)timesec * 4, 1);
	display6->displayNumber((float)timesec * 5, 1);
	display7->displayNumber((float)timesec * 6, 1);
	ldisplay1->displayNumber((float)timesec, 4);
	ldisplay2->displayNumber((float)(1000 - timesec), 4);
}

void setup() {
	Serial.begin(115200);

	shiftChain = factory.createShiftRegisterChain(LATCH_PIN, DATA_PIN, CLOCK_PIN);

	ldisplay1 = factory.createSegmentDisplay(shiftChain, 8);
	ldisplay2 = factory.createSegmentDisplay(shiftChain, 8);
	bargraph7 = factory.createBargraph(shiftChain, 10);
	display7 = factory.createSegmentDisplay(shiftChain, 4);
	bargraph6 = factory.createBargraph(shiftChain, 10);
	display6 = factory.createSegmentDisplay(shiftChain, 4);
	bargraph5 = factory.createBargraph(shiftChain, 10);
	display5 = factory.createSegmentDisplay(shiftChain, 4);
	bargraph4 = factory.createBargraph(shiftChain, 10);
	display4 = factory.createSegmentDisplay(shiftChain, 4);
	bargraph3 = factory.createBargraph(shiftChain, 10);
	display3 = factory.createSegmentDisplay(shiftChain, 4);
	bargraph2 = factory.createBargraph(shiftChain, 10);
	display2 = factory.createSegmentDisplay(shiftChain, 4);
	bargraph1 = factory.createBargraph(shiftChain, 10);
	display1 = factory.createSegmentDisplay(shiftChain, 4);

	reversed = false;

	factory.createPeriodic(10, updateDisplays);
	factory.createPeriodic(10, updateBargraphs);
	factory.createPeriodic(11000, toggleReverse);
}

void loop() {
	factory.update();
}