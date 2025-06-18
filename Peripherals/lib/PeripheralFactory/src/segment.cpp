#include "segment.h"
#include <Arduino.h> // For sprintf, strlen

// Constructor
Segment::Segment(int dataPin, int clkPin, int csPin, int numDevices)
    : lc(dataPin, clkPin, csPin, numDevices), totalNumDevices(numDevices) {
    totalDigits = totalNumDevices * 8;
}

void Segment::init() {
    for (int i = 0; i < totalNumDevices; i++) {
        lc.shutdown(i, false);
        lc.setIntensity(i, 8);
        lc.clearDisplay(i);
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
        return;
    }

    int deviceIndex = overallDigitIndex / 8;

    int digitInDevice = 7 - (overallDigitIndex % 8);

    if (deviceIndex < totalNumDevices) {
        lc.setChar(deviceIndex, digitInDevice, character, decimalPoint);
    }
}

void Segment::printString(const char* text) {
    clearAll();
    int len = strlen(text);
    int currentOverallDigit = 0;

    for (int i = 0; i < len && currentOverallDigit < totalDigits; ++i) {
        char charToDisplay = text[i];
        bool dp = false;

        if ((i + 1 < len) && text[i + 1] == '.') {
            dp = true;
            i++;
        }

        setChar(currentOverallDigit, charToDisplay, dp);
        currentOverallDigit++;
    }
}

void Segment::printNumber(long number) {
    char buf[16];
    sprintf(buf, "%ld", number);
    printString(buf);
}

LedControl& Segment::getLedControlInstance() {
    return lc;
}

void Segment::displayPower(int logicalDisplayNum, int value) {
    if (logicalDisplayNum < 0 || logicalDisplayNum >= (totalNumDevices * 2)) {
        return;
    }
    int startOverallDigit = logicalDisplayNum * 4;

    char buffer[5];

    if (value >= 0 && value <= 9999) {
        sprintf(buffer, "%4d", value);
    } else {
        strcpy(buffer, "----");
    }

    for (int k = 0; k < 4; ++k) {
        setChar(startOverallDigit + k, buffer[k], false);
    }
}

void Segment::displayPower(int logicalDisplayNum, float value) {
    if (logicalDisplayNum < 0 || logicalDisplayNum >= (totalNumDevices * 2)) {
        return;
    }
    int startOverallDigit = logicalDisplayNum * 4;

    char chars[4];
    bool dps[4] = {false, false, false, false};

    if (value >= 0.0f && value < 1000.0f) {
        int integerPart = (int)value;
        int firstDecimalDigit = (int)(value * 10.0f + 0.5f) % 10;

        chars[3] = firstDecimalDigit + '0';
        chars[2] = (integerPart % 10) + '0';
        chars[1] = (integerPart < 10) ? ' ' : ((integerPart / 10) % 10) + '0';
        chars[0] = (integerPart < 100) ? ' ' : ((integerPart / 100) % 10) + '0';
        dps[2] = true;

    } else if (value >= 1000.0f && value <= 9999.0f) {
        int intVal = (int)(value + 0.5f);
        char buffer[5];
        sprintf(buffer, "%4d", intVal);
        for(int k=0; k<4; ++k) chars[k] = buffer[k];

    } else {
        for(int k=0; k<4; ++k) chars[k] = '-';
    }

    for (int k = 0; k < 4; ++k) {
        setChar(startOverallDigit + k, chars[k], dps[k]);
    }
}
