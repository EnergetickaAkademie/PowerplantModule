#include "encoder.h"
#include <Arduino.h>

// Initialize the static maps
std::map<ESPRotary*, Encoder*> Encoder::_rotary_map;
std::map<Button2*, Encoder*> Encoder::_button_map;

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal, int16_t maxVal, int16_t steps_per_click,
	bool enable_speedup, unsigned int speedup_increment, unsigned int speedup_interval)
	: _rotary(pinA, pinB, steps_per_click, minVal, maxVal),
	  _button(pinSW),
	  _currentPosition(minVal),
	  _buttonPressed(false)
{
	_rotary_map[&_rotary] = this;
	_button_map[&_button] = this;

	_rotary.resetPosition(_currentPosition);
	_rotary.setChangedHandler(rotation_callback);
	_button.begin(_button.getPin());
	_button.setClickHandler(button_callback);
	_button.setLongClickHandler(button_callback);

	_rotary.enableSpeedup(enable_speedup);
	_rotary.setSpeedupIncrement(speedup_increment);
	_rotary.setSpeedupInterval(speedup_interval);
}

Encoder::~Encoder() {
	_rotary_map.erase(&_rotary);
	_button_map.erase(&_button);
}

void Encoder::update() {
	_rotary.loop();
	_button.loop();
}

void Encoder::rotation_callback(ESPRotary &r) {
	Encoder* instance = _rotary_map[&r];
	if (instance) {
		instance->_currentPosition = r.getPosition();
	}
}

void Encoder::setRange(int16_t minVal, int16_t maxVal) {
	if (minVal < maxVal) {
		_rotary.setLowerBound(minVal);
		_rotary.setUpperBound(maxVal);
	}
}

int16_t Encoder::getUpperBound() {
	return _rotary.getUpperBound();
}

int16_t Encoder::getLowerBound() {
	return _rotary.getLowerBound();
}

void Encoder::button_callback(Button2& b) {
	Encoder* instance = _button_map[&b];
	if (instance) {
		instance->_buttonPressed = true;
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

void Encoder::reverse() {
	_rotary.setIncrement(-_rotary.getIncrement());
}