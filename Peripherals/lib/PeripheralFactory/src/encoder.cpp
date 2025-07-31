#include "encoder.h"
#include <Arduino.h>

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal, int16_t maxVal)
    : _rotary(pinA, pinB, pinSW) 
{
    // Set encoder type - assuming it has pull-ups (common for modules)
    _rotary.setEncoderType(EncoderType::HAS_PULLUP);
    
    // Set boundaries using the correct method
    _rotary.setBoundaries(minVal, maxVal, false); 
    
    // Set the knob callback to track the current value
    _rotary.onTurned([this](long value) { 
        _currentValue = value; 
    });
    
    // Set the button callback - it expects a function that takes unsigned long duration
    _rotary.onPressed([this](unsigned long duration) { this->button_callback(); });
    
    // Initialize interrupts
    _rotary.begin();
    
    // Store values for getUpperBound and initial state
    _maxVal = maxVal;
    _currentValue = minVal;
}

int16_t Encoder::getValue() {
    return _currentValue;
}

void Encoder::setValue(int16_t value) {
    // Since the library doesn't have a direct setValue, we can only update our internal value
    // The actual encoder position is managed by the library based on physical turns
    _currentValue = value;
}

bool Encoder::isButtonPressed() {
    if (_buttonWasPressed) {
        _buttonWasPressed = false; // Reset the flag after it's been read
        return true;
    }
    return false;
}

int16_t Encoder::getUpperBound() {
    return _maxVal;
}

void Encoder::reverse() {
    // Toggle the reverse flag - this affects how we interpret turns
    _reversed = !_reversed;
    // Note: The actual reversal logic would need to be implemented in the onTurned callback
    // For now, we just toggle the flag for future use
}

void Encoder::button_callback() {
    _buttonWasPressed = true;
}