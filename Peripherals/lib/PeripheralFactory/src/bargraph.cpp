#include "bargraph.h"

Bargraph::Bargraph(NumLeds numLeds) : _numLeds(numLeds.val), _reversed(false) {
	_registerCount = (numLeds.val + 7) / 8;

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

	for (int i = 0; i < _registerCount; ++i) {
		_shiftData[i] = 0x00;
	}

	for (int i = 0; i < value; ++i) {
		int led_index = _reversed ? (_numLeds - 1 - i) : i;

		uint8_t byteIndex = (_registerCount - 1) - (led_index / 8);
		uint8_t bitIndex = led_index % 8;

		if (byteIndex < _registerCount) {
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

void Bargraph::init() {
	for (int i = 0; i < _registerCount; ++i) {
		_shiftData[i] = 0x00;
	}
}

const byte* Bargraph::getShiftData() const {
	return _shiftData;
}

uint8_t Bargraph::getRegisterCount() const {
	return _registerCount;
}