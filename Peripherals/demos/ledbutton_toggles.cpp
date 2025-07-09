#include <Arduino.h>
#include "PeripheralFactory.h"

PeripheralFactory factory;

LEDButton* btnG = nullptr;
LEDButton* btnR = nullptr;

LED* led1 = nullptr;
LED* led2 = nullptr;

void RedOff() {
	btnR->setToggleState(false);
}

void GreenOff() {
	btnG->setToggleState(false);
}

void UpdateLEDState() {
	led1->setState(btnR->getToggleState());
	led2->setState(btnG->getToggleState());
}

void setup(){
	Serial.begin(115200);

	btnR = factory.createLEDButton(5, 4);
	btnG = factory.createLEDButton(7, 6);

	led1 = factory.createLed(15);
	led2 = factory.createLed(16);

	if (btnR) {
		btnR->setMode(LEDButtonMode::TOGGLE);
		btnR->addUpdateFunction(GreenOff, UpdateFunction::TOGGLE);
		btnR->addUpdateFunction(UpdateLEDState, UpdateFunction::TOGGLE);
	}

	if (btnG) {
		btnG->setMode(LEDButtonMode::TOGGLE);
		btnG->addUpdateFunction(RedOff, UpdateFunction::TOGGLE);
		btnG->addUpdateFunction(UpdateLEDState, UpdateFunction::TOGGLE);
	}

	factory.init();
}


void loop() {
	factory.update();
}