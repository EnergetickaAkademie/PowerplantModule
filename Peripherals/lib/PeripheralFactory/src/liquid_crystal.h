#ifndef LIQUID_CRYSTAL_H
#define LIQUID_CRYSTAL_H

#include "Peripheral.h"
#include <LiquidCrystal_I2C.h>

/**
 * @brief A peripheral wrapper for a standard I2C Liquid Crystal Display.
 * This class provides a simple interface to control an I2C LCD using the LiquidCrystal_I2C library.
 * 
 * `LiquidCrystal liquidCrystal(uint8_t i2c_addr, uint8_t cols, uint8_t rows);`
 * @param i2c_addr The I2C address of the LCD (usually 0x27 or 0x3F).
 * @param cols The number of columns the display has.
 * @param rows The number of rows the display has.
 * 
 */
class LiquidCrystal : public Peripheral {
public:
	/**
	 * @brief Construct a new Liquid Crystal object.
	 * @param i2c_addr The I2C address of the LCD (usually 0x27 or 0x3F).
	 * @param cols The number of columns the display has (e.g., 16 or 20).
	 * @param rows The number of rows the display has (e.g., 2 or 4).
	 */
	LiquidCrystal(I2cAddress i2c_addr, NumCols cols, NumRows rows);
	LiquidCrystal(uint8_t i2c_addr, uint8_t cols, uint8_t rows): LiquidCrystal(I2cAddress{i2c_addr}, NumCols{cols}, NumRows{rows}) {}

	// --- Implementation for the Peripheral interface ---
	void init() override;
	void update() override;

	// --- LCD-specific methods ---
	void begin(uint8_t cols, uint8_t rows, uint8_t charsize = 0x00);
	void clear();
	void home();
	void noDisplay();
	void display();
	void noBlink();
	void blink();
	void noCursor();
	void cursor();
	void scrollDisplayLeft();
	void scrollDisplayRight();
	void printLeft();
	void printRight();
	void leftToRight();
	void rightToLeft();
	void shiftIncrement();
	void shiftDecrement();
	void noBacklight();
	void backlight();
	void autoscroll();
	void noAutoscroll();
	void createChar(uint8_t, uint8_t[]);
	void createChar(uint8_t location, const char *charmap);
	void setCursor(uint8_t, uint8_t);
	void command(uint8_t);
	void oled_init();

	// Print overloads
	void print(const char* str);
	void print(int number);

	// Compatibility API function aliases
	void blink_on();
	void blink_off();
	void cursor_on();
	void cursor_off();
	void setBacklight(uint8_t new_val);
	void load_custom_character(uint8_t char_num, uint8_t *rows);
	void printstr(const char[]);

private:
	LiquidCrystal_I2C _lcd; // The underlying library object
	bool _backlight_on;
};

#endif // LIQUID_CRYSTAL_H
