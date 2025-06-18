#include "buzzer.h"

Buzzer::Buzzer(uint8_t pin) 
    : _pin(pin), _isBuzzing(false), _startTime(0), _duration(0) {}

void Buzzer::init() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW); // Ensure buzzer is off initially
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
    // Check if the buzzer is active and if the duration has passed
    if (_isBuzzing && (millis() - _startTime >= _duration)) {
        digitalWrite(_pin, LOW); // Turn off the buzzer
        _isBuzzing = false;
    }
}
