#include <Arduino.h>
#include "PeripheralFactory.h"

#define LATCH 15
#define DIN   16
#define CLK   17

#define ENC1_PIN_A 4
#define ENC1_PIN_B 5
#define ENC1_PIN_SW 6

#define ENC2_PIN_A 7
#define ENC2_PIN_B 8
#define ENC2_PIN_SW 18

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

	encoder1 = factory.createEncoder(ENC1_PIN_A, ENC1_PIN_B, ENC1_PIN_SW, 0, 1200);

	encoder2 = factory.createEncoder(ENC2_PIN_A, ENC2_PIN_B, ENC2_PIN_SW, 0, 500);

	shiftRegisterChain = factory.createShiftRegisterChain(LATCH, DIN, CLK);

	bargraph2 = factory.createBargraph(shiftRegisterChain, 10);
	segmentDisplay2 = factory.createSegmentDisplay(shiftRegisterChain, 4);
	bargraph1 = factory.createBargraph(shiftRegisterChain, 10);
	segmentDisplay1 = factory.createSegmentDisplay(shiftRegisterChain, 4);

	factory.createPeriodic(10, std::bind(updateDisplay, encoder1, bargraph1, segmentDisplay1));
	factory.createPeriodic(10, std::bind(updateDisplay, encoder2, bargraph2, segmentDisplay2));
}

void loop() {
	factory.update();
}