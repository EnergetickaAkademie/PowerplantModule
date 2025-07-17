#include <Arduino.h>
#include "PeripheralFactory.h"

PeripheralFactory factory;

LEDButton* btnG = nullptr;
LEDButton* btnR = nullptr;

LED* led1 = nullptr;
LED* led2 = nullptr;

void UpdateLEDState() {
	led1->setState(btnR->getToggleState());
	led2->setState(btnG->getToggleState());
}

void setup(){
	Serial.begin(115200);

	ShiftRegisterChain* chain = factory.create<ShiftRegisterChain>(LatchPin{8}, DataPin{9}, ClockPin{10});

	Bargraph* bargraph = factory.create<Bargraph>(NumLeds{16});
	SegmentDisplay* display = factory.create<SegmentDisplay>(NumDigits{4});
	chain->addDevice(bargraph);
	chain->addDevice(display);

	btnR = factory.create<LEDButton>(ButtonPin{5}, LedPin{4});
	btnG = factory.create<LEDButton>(ButtonPin{7}, LedPin{6});

	OLEDDisplay* oled = factory.create<OLEDDisplay>(Width{128}, Height{64}, &Wire, RstPin{-1});

	led1 = factory.create<LED>(Pin{15});
	led2 = factory.create<LED>(Pin{16});

	btnR->setMode(LEDButtonMode::TOGGLE);
	btnR->addUpdateFunction([&](){btnG->setToggleState(false);}, UpdateFunction::TOGGLE);
	btnR->addUpdateFunction(UpdateLEDState, UpdateFunction::TOGGLE);

	btnG->setMode(LEDButtonMode::TOGGLE);
	btnG->addUpdateFunction([&](){btnG->setToggleState(false);}, UpdateFunction::TOGGLE);
	btnG->addUpdateFunction(UpdateLEDState, UpdateFunction::TOGGLE);

	factory.init();
}


void loop() {
	factory.update();
}