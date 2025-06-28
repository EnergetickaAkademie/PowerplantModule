#include "bargraph.h"

Bargraph::Bargraph(uint8_t numLeds) : _numLeds(numLeds) {
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

	for (int i = 0; i < _registerCount; ++i) {
		_shiftData[i] = 0x00;
	}

	for (int i = 0; i < value; ++i) {
		uint8_t byteIndex = i / 8;
		uint8_t bitIndex = i % 8;

		_shiftData[byteIndex] |= (1 << bitIndex);
	}
}

const byte* Bargraph::getShiftData() const {
	return _shiftData;
}

uint8_t Bargraph::getRegisterCount() const {
	return _registerCount;
}