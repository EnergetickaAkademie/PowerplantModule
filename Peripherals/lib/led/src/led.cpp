#include "led.h"

Led::Led(int pin) {
    _pin = pin;
    _blinking = false;
    _blinkInterval = 0;
    _previousMillisBlink = 0;
    _ledStateBlink = LOW;
}

void Led::begin() {
    pinMode(_pin, OUTPUT);
    off(); // Default to off
}

void Led::on() {
    _blinking = false; // Stop blinking if it was
    digitalWrite(_pin, HIGH);
}

void Led::off() {
    _blinking = false; // Stop blinking if it was
    digitalWrite(_pin, LOW);
}

void Led::startBlink(unsigned long interval) {
    _blinkInterval = interval;
    _blinking = true;
    _previousMillisBlink = millis(); // Reset timer for blinking
    _ledStateBlink = LOW; // Start with LED off, then it will turn on
    digitalWrite(_pin, _ledStateBlink);
}

void Led::stopBlink(bool leaveOn) {
    _blinking = false;
    if (leaveOn) {
        digitalWrite(_pin, HIGH);
    } else {
        digitalWrite(_pin, LOW);
    }
}

void Led::update() {
    if (_blinking) {
        unsigned long currentMillis = millis();
        if (currentMillis - _previousMillisBlink >= _blinkInterval) {
            _previousMillisBlink = currentMillis;
            // Toggle LED state
            if (_ledStateBlink == LOW) {
                _ledStateBlink = HIGH;
            } else {
                _ledStateBlink = LOW;
            }
            digitalWrite(_pin, _ledStateBlink);
        }
    }
}
