#include "led.h"

LED::LED(Pin pin) 
	: _pin(pin.val), _isBlinking(false), _ledState(LOW), _blinkInterval(0), _lastToggleTime(0), _brightness(255) {}

void LED::init() {
	pinMode(_pin, OUTPUT);
	analogWrite(_pin, 0);
}

void LED::setBrightness(uint8_t brightness) {
	_brightness = brightness;
	if (!_isBlinking && _ledState == HIGH) {
		analogWrite(_pin, _brightness);
	}
}

void LED::on() {
	_isBlinking = false;
	_ledState = HIGH;
	analogWrite(_pin, _brightness);
}

void LED::off() {
	_isBlinking = false;
	_ledState = LOW;
	analogWrite(_pin, 0);
}

void LED::startBlink(unsigned long interval) {
	_blinkInterval = interval;
	_isBlinking = true;
	_lastToggleTime = millis();
}

void LED::stopBlink() {
	_isBlinking = false;
	analogWrite(_pin, 0);
}

void LED::update() {
	if (_isBlinking && (millis() - _lastToggleTime >= _blinkInterval)) {
		_ledState = !_ledState;
		analogWrite(_pin, _ledState ? _brightness : 0);
		_lastToggleTime = millis();
	}
}

void LED::setState(bool state) {
	_isBlinking = false;
	_ledState = state;
	analogWrite(_pin, _ledState ? _brightness : 0);
}

void LED::toggle() {
	setState(!_ledState);
}