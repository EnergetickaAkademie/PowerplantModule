#ifndef SHIFT_REGISTER_CHAIN_H
#define SHIFT_REGISTER_CHAIN_H

#include "Peripheral.h"
#include "shift_register_device.h"
#include <vector>

/**
 * A shift register chain device that can control multiple devices in a daisy-chained manner.
 * @brief Construct a new Shift Register Chain object.
 * 
 * `ShiftRegisterChain chain(uint8_t latchPin, uint8_t dataPin, uint8_t clockPin);`
 * @param latchPin The pin connected to the latch of the shift register.
 * @param dataPin The pin connected to the data input of the shift register.
 * @param clockPin The pin connected to the clock of the shift register.
 */
class ShiftRegisterChain : public Peripheral {
public:

	ShiftRegisterChain(LatchPin latchPin, DataPin dataPin, ClockPin clockPin);
	ShiftRegisterChain(uint8_t latchPin, uint8_t dataPin, uint8_t clockPin)
		: ShiftRegisterChain(LatchPin{latchPin}, DataPin{dataPin}, ClockPin{clockPin}) {}

	void init() override;
	void update() override;

	void addDevice(ShiftRegisterDevice* device);

private:
	uint8_t _latchPin, _dataPin, _clockPin;
	std::vector<ShiftRegisterDevice*> _devices;
};

#endif // SHIFT_REGISTER_CHAIN_H