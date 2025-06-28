#ifndef SHIFT_REGISTER_DEVICE_H
#define SHIFT_REGISTER_DEVICE_H

#include <Arduino.h>

// Abstract base class for any device connected to a ShiftRegisterChain.
class ShiftRegisterDevice {
public:
    virtual ~ShiftRegisterDevice() {}

    // Called by the chain controller to get this device's current data.
    virtual const byte* getShiftData() const = 0;

    // Called by the chain controller to know how many bytes (registers) this device uses.
    virtual uint8_t getRegisterCount() const = 0;

    // For internal logic, like multiplexing a 7-segment display.
    virtual void update() {} 
};

#endif // SHIFT_REGISTER_DEVICE_H