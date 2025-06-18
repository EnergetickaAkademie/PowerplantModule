#include "encoder.h"
#include <Arduino.h>

// Initialize the static instance pointer
Encoder* Encoder::_instance = nullptr;

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal, int16_t maxVal, int16_t steps_per_click)
    : _rotary(pinA, pinB, steps_per_click, minVal, maxVal), 
      _pinSW(pinSW),
      _currentPosition(minVal),
      _lastButtonState(HIGH),
      _lastDebounceTime(0),
      _buttonWasPressed(false)
{
    // Set the static instance pointer to this object
    _instance = this;
}

void Encoder::init() {
    pinMode(_pinSW, INPUT_PULLUP);
    // Set the internal position and attach the callback handler
    _rotary.resetPosition(_currentPosition);
    _rotary.setChangedHandler(_static_rotation_callback);

    // --- ADD THESE LINES TO ENABLE SPEEDUP ---
    // Enable the speedup feature
    _rotary.enableSpeedup(true); 
    // Set the increment to 5 when speedup is active
    _rotary.setSpeedupIncrement(5); 
    // Set the interval threshold to 75ms (the library's default)
    _rotary.setSpeedupInterval(75); 
}

// This is the static callback function that the ESPRotary library will call.
// It runs in an interrupt, so it must be fast.
void IRAM_ATTR Encoder::_static_rotation_callback(ESPRotary &r) {
    if (_instance) {
        // Use the instance pointer to update the member variable of our object
        _instance->_currentPosition = r.getPosition();
    }
}

void Encoder::update() {
    // The main update loop still needs to call the library's loop function
    _rotary.loop();

    // --- Button debounce logic ---
    uint8_t reading = digitalRead(_pinSW);
    if (reading != _lastButtonState) {
        _lastDebounceTime = millis();
    }
    if ((millis() - _lastDebounceTime) > 50) {
        if (reading == LOW && _lastButtonState == HIGH) {
            _buttonWasPressed = true;
        }
    }
    _lastButtonState = reading;
}

int16_t Encoder::getValue() {
    // Return the value that was updated by the callback
    return _currentPosition;
}

void Encoder::setValue(int16_t value) {
    _rotary.resetPosition(value);
    _currentPosition = _rotary.getPosition(); // Ensure our internal value is also updated
}

bool Encoder::isButtonPressed() {
    if (_buttonWasPressed) {
        _buttonWasPressed = false;
        return true;
    }
    return false;
}