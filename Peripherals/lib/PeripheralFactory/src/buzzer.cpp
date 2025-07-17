#include "buzzer.h"

Buzzer::Buzzer(Pin pin) 
	: _pin(pin.val), _isBuzzing(false), _startTime(0), _duration(0) {}

void Buzzer::init() {
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}

void Buzzer::buzz(unsigned long duration_ms) {
	if (duration_ms > 0) {
		_duration = duration_ms;
		_isBuzzing = true;
		_startTime = millis();
		digitalWrite(_pin, HIGH);
	}
}

void Buzzer::update() {
	if (_isBuzzing && (millis() - _startTime >= _duration)) {
		digitalWrite(_pin, LOW);
		_isBuzzing = false;
	}
}
