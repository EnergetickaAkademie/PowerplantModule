#include "segment.h"
#include <Arduino.h> // For sprintf, strlen

// Constructor
Segment::Segment(int dataPin, int clkPin, int csPin, int numDevices)
    : lc(dataPin, clkPin, csPin, numDevices), totalNumDevices(numDevices) {
    totalDigits = totalNumDevices * 8; // Each MAX7219 controls 8 digits
}

void Segment::init() { // Renamed from begin()
    for (int i = 0; i < totalNumDevices; i++) {
        lc.shutdown(i, false); // Wake up MAX7219 from power-saving mode
        lc.setIntensity(i, 8); // Set a default brightness (0-15)
        lc.clearDisplay(i);    // Clear display register
    }
}

void Segment::setBrightness(int intensity) {
    if (intensity < 0) intensity = 0;
    if (intensity > 15) intensity = 15;
    for (int i = 0; i < totalNumDevices; i++) {
        lc.setIntensity(i, intensity);
    }
}

void Segment::clearAll() {
    for (int i = 0; i < totalNumDevices; i++) {
        lc.clearDisplay(i);
    }
}

void Segment::setChar(int overallDigitIndex, char character, bool decimalPoint) {
    if (overallDigitIndex < 0 || overallDigitIndex >= totalDigits) {
        return; // Index out of bounds
    }

    int deviceIndex = overallDigitIndex / 8;
    // To reverse the digit order within each device:
    // Instead of (overallDigitIndex % 8), it becomes (7 - (overallDigitIndex % 8))
    int digitInDevice = 7 - (overallDigitIndex % 8);

    if (deviceIndex < totalNumDevices) {
        lc.setChar(deviceIndex, digitInDevice, character, decimalPoint);
    }
}

void Segment::printString(const char* text) {
    clearAll();
    int len = strlen(text);
    int currentOverallDigit = 0;

    // To display the string from left-to-right visually, but map to hardware correctly:
    // We iterate through the string normally, but the `setChar` will handle the reversed
    // digit order within each physical MAX7219 module.

    for (int i = 0; i < len && currentOverallDigit < totalDigits; ++i) {
        char charToDisplay = text[i];
        bool dp = false;

        // Check for decimal point following the character
        if ((i + 1 < len) && text[i + 1] == '.') {
            dp = true;
            i++; // Consume the decimal point character, it will be handled by setChar
        }

        setChar(currentOverallDigit, charToDisplay, dp);
        currentOverallDigit++;
    }
}

void Segment::printNumber(long number) {
    char buf[16]; // Sufficient for long, sign, and null terminator
    sprintf(buf, "%ld", number);
    printString(buf);
}

LedControl& Segment::getLedControlInstance() {
    return lc;
}

void Segment::displayPower(int logicalDisplayNum, int value) {
    if (logicalDisplayNum < 0 || logicalDisplayNum >= (totalNumDevices * 2)) {
        return; // Invalid logical display number
    }
    int startOverallDigit = logicalDisplayNum * 4;

    char buffer[5]; // 4 chars for digits + null terminator

    if (value >= 0 && value <= 9999) {
        sprintf(buffer, "%4d", value); // Right-justifies with leading spaces, e.g., "  23", "1234"
    } else {
        strcpy(buffer, "----"); // Value out of range
    }

    for (int k = 0; k < 4; ++k) {
        setChar(startOverallDigit + k, buffer[k], false); // No decimal point for the integer version
    }
}

void Segment::displayPower(int logicalDisplayNum, float value) {
    if (logicalDisplayNum < 0 || logicalDisplayNum >= (totalNumDevices * 2)) {
        return; // Invalid logical display number
    }
    int startOverallDigit = logicalDisplayNum * 4;

    char chars[4]; // To hold the characters for the 4 digits
    bool dps[4] = {false, false, false, false}; // To hold decimal point states for each digit position

    if (value >= 0.0f && value < 1000.0f) {
        // Format as XXX.X (e.g., "123.4", " 23.4", "  3.4")
        int integerPart = (int)value;
        // Get the first decimal digit, rounded. (value * 10.0f + 0.5f) scales and rounds.
        // Then % 10 gets the last digit of that, which is our first decimal place.
        int firstDecimalDigit = (int)(value * 10.0f + 0.5f) % 10;

        chars[3] = firstDecimalDigit + '0';
        chars[2] = (integerPart % 10) + '0';
        chars[1] = (integerPart < 10) ? ' ' : ((integerPart / 10) % 10) + '0';
        chars[0] = (integerPart < 100) ? ' ' : ((integerPart / 100) % 10) + '0';
        dps[2] = true; // Decimal point is after the character at index 2 (3rd position from left)

    } else if (value >= 1000.0f && value <= 9999.0f) {
        int intVal = (int)(value + 0.5f); // Round to nearest integer
        char buffer[5];
        sprintf(buffer, "%4d", intVal); // Format as 4-digit integer, right-justified
        for(int k=0; k<4; ++k) chars[k] = buffer[k];
        // dps remains all false for this case
    } else {
        // Value is out of displayable range (e.g., negative, too large, NaN)
        for(int k=0; k<4; ++k) chars[k] = '-';
        // dps remains all false for this case
    }

    // Display the characters on the segment
    for (int k = 0; k < 4; ++k) {
        setChar(startOverallDigit + k, chars[k], dps[k]);
    }
}
