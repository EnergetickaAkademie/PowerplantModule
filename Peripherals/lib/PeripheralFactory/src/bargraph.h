#ifndef BARGRAPH_H
#define BARGRAPH_H

#include "shift_register_device.h"

/**
 * @brief A flexible bargraph device controlled by one or more shift registers.
 * 
 * `Bargraph bargraph(uint8_t numLeds);`
 * @param numLeds The total number of LEDs in the bargraph.
 */
class Bargraph : public ShiftRegisterDevice {
public:
	/**
	 * @brief Construct a new Bargraph object.
	 * @param numLeds The total number of LEDs in the bargraph.
	 */
	Bargraph(NumLeds numLeds = {16});

	Bargraph(uint8_t numLeds) : Bargraph(NumLeds{numLeds}) {}

	/**
	 * @brief Destructor to free the dynamically allocated memory.
	 */
	~Bargraph();

	/**
	 * @brief Sets the number of LEDs to light up.
	 * @param value The number of LEDs to turn on, from 0 to numLeds.
	 */
	void setValue(uint8_t value);

	/**
	 * @brief Sets the display direction of the bargraph.
	 * @param reversed If true, the bargraph will fill from top to bottom.
	 */
	void setReversed(bool reversed);

	/**
	 * @brief Sets the raw byte data for the bargraph LEDs.
	 * @param data A pointer to an array of bytes representing the LED states.
	 * @param count The number of bytes in the data array.
	 */
	void setRawData(const byte* data, uint8_t count);

	// --- Implementations for the Peripheral interface ---
	void init() override;

	// --- Implementations for the ShiftRegisterDevice interface ---
	const byte* getShiftData() const override;
	uint8_t getRegisterCount() const override;

private:
	uint8_t _numLeds;
	uint8_t _registerCount;
	byte* _shiftData; // A raw pointer to a dynamically allocated array
	bool _reversed; // Flag to control display direction
};

#endif // BARGRAPH_H