#include "ledbutton.h"
#include <functional> // Required for std::bind

LEDButton::LEDButton(ButtonPin buttonPin, LedPin ledPin)
	: _button(buttonPin.val), _mode(LEDButtonMode::MANUAL), _toggleState(false) {

	_led = new LED(ledPin);
}

LEDButton::~LEDButton() {
	delete _led;
	_led = nullptr;
}

void LEDButton::init() {
	if (_led) {
		_led->init();
	}

	setMode(_mode);
}

void LEDButton::update() {
	_button.loop();
	if (_led) {
		_led->update();
	}
}

void LEDButton::setMode(LEDButtonMode mode) {
	_mode = mode;

	_button.setPressedHandler(nullptr);
	_button.setReleasedHandler(nullptr);
	_button.setClickHandler(nullptr);

	switch (_mode) {
		case LEDButtonMode::FOLLOW:
			_button.setPressedHandler(std::bind(&LEDButton::_followPressed, this, std::placeholders::_1));
			_button.setReleasedHandler(std::bind(&LEDButton::_followReleased, this, std::placeholders::_1));
			break;
		case LEDButtonMode::TOGGLE:
			_button.setClickHandler(std::bind(&LEDButton::_toggleClicked, this, std::placeholders::_1));
			_button.setLongClickHandler(std::bind(&LEDButton::_toggleClicked, this, std::placeholders::_1));
			break;
		case LEDButtonMode::MANUAL:
			break;
	}
}

LED* LEDButton::getLED() {
	return _led;
}

Button2* LEDButton::getButton() {
	return &_button;
}

void LEDButton::_followPressed(Button2& b) {
	if (_led) {
		_led->on();
	}

	for (const auto& func : pressFuncs) {
		func();
	}
}

void LEDButton::_followReleased(Button2& b) {
	if (_led) {
		_led->off();
	}

	for (const auto& func : releaseFuncs) {
		func();
	}
}

void LEDButton::_toggleClicked(Button2& b) {
	if (_led) {
		_toggleState = !_toggleState;
		_led->setState(_toggleState);
	}

	for (const auto& func : toggleFuncs) {
		func();
	}
}

void LEDButton::addUpdateFunction(std::function<void()> func, UpdateFunction type) {
	switch (type) {
		case UpdateFunction::PRESS:
			pressFuncs.push_back(func);
			break;
		case UpdateFunction::RELEASE:
			releaseFuncs.push_back(func);
			break;
		case UpdateFunction::TOGGLE:
			toggleFuncs.push_back(func);
			break;
	}
}

bool LEDButton::getToggleState(){
	return _toggleState;
}

void LEDButton::setToggleState(bool state) {
	_toggleState = state;
	if (_led) {
		_led->setState(_toggleState);
	}
}

void LEDButton::clearUpdateFunctions() {
	toggleFuncs.clear();
	pressFuncs.clear();
	releaseFuncs.clear();
}

void LEDButton::clearUpdateFunctions(UpdateFunction type) {
	switch (type) {
		case UpdateFunction::PRESS:
			pressFuncs.clear();
			break;
		case UpdateFunction::RELEASE:
			releaseFuncs.clear();
			break;
		case UpdateFunction::TOGGLE:
			toggleFuncs.clear();
			break;
	}
}