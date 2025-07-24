#include "bargraph.h"

Bargraph::Bargraph(uint8_t numLeds) : _numLeds(numLeds), _reversed(false) {
	_registerCount = (numLeds + 7) / 8;

	_shiftData = new byte[_registerCount];

	for (int i = 0; i < _registerCount; ++i) {
		_shiftData[i] = 0x00;
	}
}

Bargraph::~Bargraph() {
	delete[] _shiftData;
}

void Bargraph::setValue(uint8_t value) {
	if (value > _numLeds) {
		value = _numLeds;
	}

	// First, clear all data across all registers.
	for (int i = 0; i < _registerCount; ++i) {
		_shiftData[i] = 0x00;
	}

	// Now, light up the correct number of LEDs.
	for (int i = 0; i < value; ++i) {
		// 'i' represents the step (0 to value-1)

		// If reversed, count from the top LED down. Otherwise, count from the bottom up.
		int led_index = _reversed ? (_numLeds - 1 - i) : i;

		// Calculate which shift register (byte) this LED belongs to,
		// starting from the end of the array to match the data flow.
		uint8_t byteIndex = (_registerCount - 1) - (led_index / 8);
		
		// Calculate which bit within that byte corresponds to this LED.
		uint8_t bitIndex = led_index % 8;

		// Turn on the bit for that LED.
		if (byteIndex < _registerCount) {
			// This assumes LED 1 is Q0, LED 2 is Q1, etc.
			_shiftData[byteIndex] |= (1 << bitIndex);
		}
	}
}

void Bargraph::setReversed(bool reversed) {
	_reversed = reversed;
}

void Bargraph::setRawData(const byte* data, uint8_t count) {
	if (count > _registerCount) {
		count = _registerCount;
	}
	memcpy(_shiftData, data, count);
}

const byte* Bargraph::getShiftData() const {
	return _shiftData;
}

uint8_t Bargraph::getRegisterCount() const {
	return _registerCount;
}