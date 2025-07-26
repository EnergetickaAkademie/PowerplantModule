#include "encoder.h"
#include <Arduino.h>

// Initialize the static instance pointer
Encoder* Encoder::_instance = nullptr;

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal, int16_t maxVal, int16_t steps_per_click)
	: _rotary(pinA, pinB, steps_per_click, minVal, maxVal),
	  _button(pinSW),
	  _currentPosition(minVal),
	  _buttonPressed(false)
{
	_instance = this;

	_rotary.resetPosition(_currentPosition);
	_rotary.setChangedHandler(rotation_callback);
	_button.begin(_button.getPin());
	_button.setClickHandler(button_callback);
	_button.setLongClickHandler(button_callback);

	_rotary.enableSpeedup(true); 
	_rotary.setSpeedupIncrement(5); 
	_rotary.setSpeedupInterval(75); 
}

void Encoder::update() {
	_rotary.loop();
	_button.loop();
}

void IRAM_ATTR Encoder::rotation_callback(ESPRotary &r) {
	if (_instance) {
		_instance->_currentPosition = r.getPosition();
	}
}

void Encoder::button_callback(Button2& b) {
	if (_instance) {
		_instance->_buttonPressed = true;
	}
}

int16_t Encoder::getValue() {
	return _currentPosition;
}

void Encoder::setValue(int16_t value) {
	_rotary.resetPosition(value);
	_currentPosition = _rotary.getPosition();
}

bool Encoder::isButtonPressed() {
	if (_buttonPressed) {
		_buttonPressed = false;
		return true;
	}
	return false;
}