#include "button.h"

Button::Button(uint8_t pin) : _button(pin) {

}

void Button::update() {
	_button.loop();
}

void Button::setClickHandler(button_callback_t f) {
	_button.setClickHandler(f);
}

void Button::setDoubleClickHandler(button_callback_t f) {
	_button.setDoubleClickHandler(f);
}

void Button::setLongClickHandler(button_callback_t f) {
	_button.setLongClickHandler(f);
}

void Button::setTripleClickHandler(button_callback_t f) {
	_button.setTripleClickHandler(f);
}
