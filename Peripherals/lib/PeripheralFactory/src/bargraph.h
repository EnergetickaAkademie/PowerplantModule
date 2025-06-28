#ifndef BARGRAPH_H
#define BARGRAPH_H

#include "shift_register_device.h"

/**
 * @brief A flexible bargraph device controlled by one or more shift registers.
 */
class Bargraph : public ShiftRegisterDevice {
public:
	/**
	 * @brief Construct a new Bargraph object.
	 * @param numLeds The total number of LEDs in the bargraph.
	 */
	Bargraph(uint8_t numLeds = 16);

	/**
	 * @brief Destructor to free the dynamically allocated memory.
	 */
	~Bargraph();

	/**
	 * @brief Sets the number of LEDs to light up.
	 * @param value The number of LEDs to turn on, from 0 to numLeds.
	 */
	void setValue(uint8_t value);

	// --- Implementations for the ShiftRegisterDevice interface ---
	const byte* getShiftData() const override;
	uint8_t getRegisterCount() const override;

private:
	uint8_t _numLeds;
	uint8_t _registerCount;
	byte* _shiftData; // A raw pointer to a dynamically allocated array
};

#endif // BARGRAPH_H