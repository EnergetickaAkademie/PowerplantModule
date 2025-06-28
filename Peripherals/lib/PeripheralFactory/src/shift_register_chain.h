#ifndef SHIFT_REGISTER_CHAIN_H
#define SHIFT_REGISTER_CHAIN_H

#include "Peripheral.h"
#include "shift_register_device.h"
#include <vector>

class ShiftRegisterChain : public Peripheral {
public:
    ShiftRegisterChain(uint8_t latchPin, uint8_t dataPin, uint8_t clockPin);

    void init() override;
    void update() override;

    // Add devices to the chain, from furthest to closest to the MCU.
    void addDevice(ShiftRegisterDevice* device);

private:
    uint8_t _latchPin, _dataPin, _clockPin;
    std::vector<ShiftRegisterDevice*> _devices;
};

#endif // SHIFT_REGISTER_CHAIN_H