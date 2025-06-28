#include "segment_display.h"
#include <string.h>
#include <stdlib.h>

const byte SegmentDisplay::digitToSegment[12] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b10000000, // 10: Decimal Point (DP)
    0b00000000  // 11: Blank
};

const byte SegmentDisplay::digitSelect[8] = {
	(byte)~(1 << 0), (byte)~(1 << 1), (byte)~(1 << 2), (byte)~(1 << 3),
	(byte)~(1 << 4), (byte)~(1 << 5), (byte)~(1 << 6), (byte)~(1 << 7)
};

SegmentDisplay::SegmentDisplay(uint8_t numDigits) 
	: _numDigits(numDigits), _currentDigit(0) {
	_digit_values = new byte[_numDigits];
    _dp_values = new bool[_numDigits];
	clear(); // Initialize display to be blank
}

SegmentDisplay::~SegmentDisplay() {
	delete[] _digit_values;
    delete[] _dp_values;
}

void SegmentDisplay::clear() {
    for (int i = 0; i < _numDigits; i++) {
        _digit_values[i] = 11; // 11 is the index for a blank display
        _dp_values[i] = false;
    }
}

void SegmentDisplay::displayNumber(long number) {
    char buffer[_numDigits + 2]; // +2 to be safe with long numbers
    ltoa(number, buffer, 10); // Convert long to a string (base 10)
    displayString(buffer);
}

void SegmentDisplay::displayString(const char* str) {
    clear();
    int len = strlen(str);
    int displayPos = _numDigits - 1; // Start from the rightmost digit

    for (int i = len - 1; i >= 0 && displayPos >= 0; i--) {
        char c = str[i];
        if (c >= '0' && c <= '9') {
            _digit_values[displayPos--] = c - '0';
        } else if (c == '.') {
            // Activate the decimal point for the digit to the right
            if (displayPos < _numDigits - 1) {
                _dp_values[displayPos + 1] = true;
            }
        } else if (c == ' ') {
            _digit_values[displayPos--] = 11; // Blank
        }
        // Other characters are ignored
    }
}

const byte* SegmentDisplay::getShiftData() const {
	return _shiftData;
}

uint8_t SegmentDisplay::getRegisterCount() const {
	return 2; // This device always uses 2 shift registers
}

void SegmentDisplay::update() {
	byte value = _digit_values[_currentDigit];
	byte segmentPattern = digitToSegment[value];

    // Add decimal point if needed
    if (_dp_values[_currentDigit]) {
        segmentPattern |= digitToSegment[10]; // OR with DP pattern
    }

	byte digitSelectPattern = digitSelect[_currentDigit];

	_shiftData[0] = digitSelectPattern;
	_shiftData[1] = segmentPattern;

	_currentDigit = (_currentDigit + 1) % _numDigits;
}