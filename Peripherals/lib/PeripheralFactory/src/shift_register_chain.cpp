#include "shift_register_chain.h"

ShiftRegisterChain::ShiftRegisterChain(uint8_t latchPin, uint8_t dataPin, uint8_t clockPin)
    : _latchPin(latchPin), _dataPin(dataPin), _clockPin(clockPin) {}

void ShiftRegisterChain::init() {
    pinMode(_latchPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
}

void ShiftRegisterChain::addDevice(ShiftRegisterDevice* device) {
    _devices.push_back(device);
}

void ShiftRegisterChain::update() {
    // First, call the internal update on all devices so they can update their state (e.g., multiplexing)
    for (ShiftRegisterDevice* device : _devices) {
        device->update();
    }

    // Now, gather the data from all devices and shift it out.
    digitalWrite(_latchPin, LOW);

    // Loop through devices in order (furthest first)
    for (ShiftRegisterDevice* device : _devices) {
        const byte* data = device->getShiftData();
        uint8_t count = device->getRegisterCount();
        // Shift out the data for this device, MSB first.
        for (int i = 0; i < count; i++) {
            shiftOut(_dataPin, _clockPin, MSBFIRST, data[i]);
        }
    }

    digitalWrite(_latchPin, HIGH);
}