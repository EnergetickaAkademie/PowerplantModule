#ifndef ENCODER_H
#define ENCODER_H

#include "Peripheral.h"
#include <ESP32RotaryEncoder.h>

class Encoder : public Peripheral {
public:
	// Simplified constructor
	Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100, uint16_t speedup_ms = 150, uint8_t speedup_multiplier = 2);
	
	// The update method is no longer needed as the library is interrupt-driven
	void update() override {}

	int16_t getValue();
	void setValue(int16_t value);
	bool isButtonPressed();
	int16_t getUpperBound();
	void reverse();
	
	// Enable/disable encoder
	void enable();
	void disable();
	
	// Speedup configuration
	void setSpeedupConfig(uint16_t threshold_ms = 100, uint8_t multiplier = 3);
	void enableSpeedup(bool enabled = true);
	
	// Public method that can be called from the ISR helper
	void button_callback();

private:
	RotaryEncoder _rotary;
	int16_t _minVal;
	int16_t _maxVal;
	bool _reversed = false;
	
	// Track the current value from the encoder callback
	volatile int16_t _currentValue = 0;
	volatile long _lastLibValue = 0; // Last value reported by the library
	
	// We need a flag for the button press, which will be set in a callback
	volatile bool _buttonWasPressed = false;
	
	// Speedup functionality
	bool _speedupEnabled = false;
	uint16_t _speedupThreshold = 100; // ms
	uint8_t _speedupMultiplier = 3;
	unsigned long _lastTurnTime = 0;

	uint8_t _stepsPerDetent = 4;  // Typically 4 pulses per physical click
	int8_t _accumulatedSteps = 0; // Track partial steps
};

#endif // ENCODER_H