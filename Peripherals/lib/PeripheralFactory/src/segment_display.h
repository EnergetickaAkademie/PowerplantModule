#ifndef SEGMENT_DISPLAY_H
#define SEGMENT_DISPLAY_H

#include "shift_register_device.h"

/**
 * @brief A logical device that controls a 7-segment display module.
 * 
 * This class manages the state of a multi-digit 7-segment display.
 * It handles number-to-segment conversion and multiplexing. It does not
 * directly control hardware pins; instead, it provides its data to a
 * ShiftRegisterChain controller.
 */
class SegmentDisplay : public ShiftRegisterDevice {
public:
    /**
     * @brief Construct a new Segment Display object.
     * @param numDigits The number of digits on the display module (e.g., 4 or 8).
     */
    SegmentDisplay(uint8_t numDigits = 4);
    ~SegmentDisplay(); // Add destructor declaration

    /**
     * @brief Sets the number to be displayed.
     * @param number The integer to display.
     */
    void displayNumber(long number);
    void displayNumber(float number, uint8_t decimalPlaces = 2);
    void displayString(const char* str);
    void clear();

    // --- Implementations for the ShiftRegisterDevice interface ---

    /**
     * @brief Gets the data to be shifted out for this device.
     * @return A const pointer to the internal data buffer (2 bytes).
     */
    const byte* getShiftData() const override;

    /**
     * @brief Gets the number of shift registers this device uses.
     * @return Always returns 2 (one for segments, one for digits).
     */
    uint8_t getRegisterCount() const override;

    /**
     * @brief Updates the internal state for multiplexing.
     * This should be called on every loop by the chain controller.
     */
    void update() override;

    void test();

private:
    static const byte digitToSegment[12];
    static const byte digitSelect[8];

    uint8_t _numDigits;
    uint8_t _currentDigit;
    byte* _digit_values; // Stores the value (0-9) for each digit position
    bool* _dp_values;    // Stores the decimal point state for each digit

    byte _shiftData[2];
};

#endif // SEGMENT_DISPLAY_H