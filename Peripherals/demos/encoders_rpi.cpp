#include <Arduino.h>
#include "PeripheralFactory.h"

#define LATCH 2
#define DIN 3
#define CLK 4

#define ENC1_PIN_A 6
#define ENC1_PIN_B 7
#define ENC2_PIN_A 8
#define ENC2_PIN_B 9

PeripheralFactory factory;

Encoder* encoder1 = nullptr;
Encoder* encoder2 = nullptr;

ShiftRegisterChain* shiftRegisterChain = nullptr;

Bargraph* bargraph2 = nullptr;
SegmentDisplay* segmentDisplay2 = nullptr;
Bargraph* bargraph1 = nullptr;
SegmentDisplay* segmentDisplay1 = nullptr;

void updateDisplay(Encoder* enc, Bargraph* bar, SegmentDisplay* seg) {
	bar->setValue(enc->getValue() * 10 / enc->getUpperBound());

	seg->displayNumber((long)enc->getValue());
}

void setup() {
	Serial.begin(115200);

	encoder1 = factory.createEncoder(ENC1_PIN_A, ENC1_PIN_B, 15, 0, 1200, 2, true, 5, 75);
	encoder2 = factory.createEncoder(ENC2_PIN_A, ENC2_PIN_B, 14, 0, 500, 2, true, 5, 75);

	shiftRegisterChain = factory.createShiftRegisterChain(LATCH, DIN, CLK);

	bargraph2 = factory.createBargraph(shiftRegisterChain, 10);
	segmentDisplay2 = factory.createSegmentDisplay(shiftRegisterChain, 4);
	bargraph1 = factory.createBargraph(shiftRegisterChain, 10);
	segmentDisplay1 = factory.createSegmentDisplay(shiftRegisterChain, 4);

	// factory.createPeriodic(100, update1);
	// factory.createPeriodic(100, update2);
	factory.createPeriodic(100, std::bind(updateDisplay, encoder1, bargraph1, segmentDisplay1));
	factory.createPeriodic(100, std::bind(updateDisplay, encoder2, bargraph2, segmentDisplay2));
}

void loop() {
	factory.update();
}