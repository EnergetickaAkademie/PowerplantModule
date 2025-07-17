#include "encoder.h"
#include <Arduino.h>

Encoder* Encoder::_instance = nullptr;

Encoder::Encoder(PinA pinA, PinB pinB, PinSW pinSW, MinVal minVal, MaxVal maxVal, Step steps_per_click)
	: _rotary(pinA.val, pinB.val, steps_per_click.val, minVal.val, maxVal.val),
	  _button(pinSW.val),
	  _currentPosition(minVal.val),
	  _buttonPressed(false)
{
	_instance = this;
}

void Encoder::init() {
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