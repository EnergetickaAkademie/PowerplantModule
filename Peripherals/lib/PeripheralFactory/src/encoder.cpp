#include "encoder.h"
#include <Arduino.h>

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal, int16_t maxVal, uint16_t speedup_ms, uint8_t speedup_multiplier)
	: _rotary(pinA, pinB, pinSW) 
{
	// Set encoder type - assuming it has pull-ups (common for modules)
	_rotary.setEncoderType(EncoderType::HAS_PULLUP);
	
	// Set boundaries using the correct method
	_rotary.setBoundaries(minVal, maxVal, false); 
	
	// Set the knob callback to track the current value with speedup
	_rotary.onTurned([this](long value) { 
		static int lastDirection = 0;
		unsigned long currentTime = millis();
		
		// Calculate the change from the last library value
		long libDelta = value - _lastLibValue;
		_lastLibValue = value;

		if (libDelta == 0) return;

		int currentDirection = (libDelta > 0) ? 1 : -1;
		int direction = _reversed ? -currentDirection : currentDirection;

		// Reset speedup if:
		// 1. Direction changed, OR
		// 2. We're near minimum and moving downward
		bool nearMinimum = (_currentValue <= (_minVal + 5)); // 5 is arbitrary threshold
		if ((lastDirection != 0 && currentDirection != lastDirection) || 
			(nearMinimum && direction < 0)) {
			_lastTurnTime = 0; // Completely reset speedup
		}
		lastDirection = currentDirection;

		// Calculate step size - always 1 when near minimum
		int stepSize = 1;
		if (_speedupEnabled && _lastTurnTime > 0 && !nearMinimum) {
			unsigned long timeDiff = currentTime - _lastTurnTime;
			if (timeDiff < _speedupThreshold) {
				stepSize = _speedupMultiplier;
			}
		}

		// Apply change
		int steps = abs(libDelta);
		long valueChange = direction * stepSize * steps;
		long newValue = _currentValue + valueChange;
		
		// Fixed boundary clamping with consistent types
		if (newValue < _minVal) newValue = _minVal;
		if (newValue > _maxVal) newValue = _maxVal;
		
		_currentValue = newValue;
		_lastTurnTime = currentTime;
	});
	
	// Set the button callback - it expects a function that takes unsigned long duration
	_rotary.onPressed([this](unsigned long duration) { this->button_callback(); });
	
	// Initialize interrupts
	_rotary.begin();
	
	// Store values for getUpperBound and initial state
	_minVal = minVal;
	_maxVal = maxVal;
	_currentValue = minVal;
	_lastLibValue = minVal;

	if(speedup_ms > 0) {
		this->enableSpeedup(true);
		this->setSpeedupConfig(speedup_ms, speedup_multiplier);
	}
}

int16_t Encoder::getValue() {
	return _currentValue;
}

void Encoder::setValue(int16_t value) {
	_currentValue = value;
	_lastLibValue = value;
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
	// Note: The actual reversal logic is implemented in the onTurned callback
}

void Encoder::enable() {
	// Enable the encoder interrupts
	_rotary.begin();
}

void Encoder::disable() {
	// Disable the encoder interrupts by calling end() if available
	// Note: The ESP32RotaryEncoder library may not have an end() method
	// In that case, we could set a flag to ignore callbacks
}

void Encoder::setSpeedupConfig(uint16_t threshold_ms, uint8_t multiplier) {
	_speedupThreshold = threshold_ms;
	_speedupMultiplier = multiplier;
}

void Encoder::enableSpeedup(bool enabled) {
	_speedupEnabled = enabled;
}

void Encoder::button_callback() {
	_buttonWasPressed = true;
}