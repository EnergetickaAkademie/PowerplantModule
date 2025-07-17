#include "PeripheralFactory.h"
#include <Arduino.h>

PeripheralFactory::PeripheralFactory() {
	_peripheralCount = 0;
}

PeripheralFactory::~PeripheralFactory() {
	for (int i = 0; i < _peripheralCount; i++) {
		delete _peripherals[i];
		_peripherals[i] = nullptr;
	}
}

void PeripheralFactory::add(Peripheral* peripheral) {
	if (_peripheralCount < MAX_PERIPHERALS) {
		_peripherals[_peripheralCount] = peripheral;
		_peripheralCount++;
	} else {
		Serial.println(F("Error: PeripheralFactory is full. Cannot add more peripherals."));
		delete peripheral;
	}
}

void PeripheralFactory::init() {
	for (int i = 0; i < _peripheralCount; i++) {
		if (_peripherals[i] != nullptr) {
			_peripherals[i]->init();
		}
	}
}

void PeripheralFactory::update() {
	for (int i = 0; i < _peripheralCount; i++) {
		if (_peripherals[i] != nullptr) {
			_peripherals[i]->update();
		}
	}
}