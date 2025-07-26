#include "liquid_crystal.h"

LiquidCrystal::LiquidCrystal(uint8_t i2c_addr, uint8_t cols, uint8_t rows)
	: _lcd(i2c_addr, cols, rows), _backlight_on(true) {

	_lcd.init();
	_lcd.backlight();
	_lcd.clear();
}

void LiquidCrystal::update() {
	// I2C LCDs are message-based and don't require a constant update loop
	// unless implementing custom animations, so this can be empty.
}

// --- LiquidCrystal_I2C API Wrappers ---

void LiquidCrystal::begin(uint8_t cols, uint8_t rows, uint8_t charsize) {
	_lcd.begin(cols, rows, charsize);
}

void LiquidCrystal::clear() {
	_lcd.clear();
}

void LiquidCrystal::home() {
	_lcd.home();
}

void LiquidCrystal::noDisplay() {
	_lcd.noDisplay();
}

void LiquidCrystal::display() {
	_lcd.display();
}

void LiquidCrystal::noBlink() {
	_lcd.noBlink();
}

void LiquidCrystal::blink() {
	_lcd.blink();
}

void LiquidCrystal::noCursor() {
	_lcd.noCursor();
}

void LiquidCrystal::cursor() {
	_lcd.cursor();
}

void LiquidCrystal::scrollDisplayLeft() {
	_lcd.scrollDisplayLeft();
}

void LiquidCrystal::scrollDisplayRight() {
	_lcd.scrollDisplayRight();
}

void LiquidCrystal::printLeft() {
	_lcd.printLeft();
}

void LiquidCrystal::printRight() {
	_lcd.printRight();
}

void LiquidCrystal::leftToRight() {
	_lcd.leftToRight();
}

void LiquidCrystal::rightToLeft() {
	_lcd.rightToLeft();
}

void LiquidCrystal::shiftIncrement() {
	_lcd.shiftIncrement();
}

void LiquidCrystal::shiftDecrement() {
	_lcd.shiftDecrement();
}

void LiquidCrystal::noBacklight() {
	_lcd.noBacklight();
	_backlight_on = false;
}

void LiquidCrystal::backlight() {
	_lcd.backlight();
	_backlight_on = true;
}

void LiquidCrystal::autoscroll() {
	_lcd.autoscroll();
}

void LiquidCrystal::noAutoscroll() {
	_lcd.noAutoscroll();
}

void LiquidCrystal::createChar(uint8_t location, uint8_t charmap[]) {
	_lcd.createChar(location, charmap);
}

void LiquidCrystal::createChar(uint8_t location, const char *charmap) {
	_lcd.createChar(location, charmap);
}

void LiquidCrystal::setCursor(uint8_t col, uint8_t row) {
	_lcd.setCursor(col, row);
}

void LiquidCrystal::command(uint8_t cmd) {
	_lcd.command(cmd);
}

void LiquidCrystal::oled_init() {
	_lcd.oled_init();
}

// --- Print overloads ---

void LiquidCrystal::print(const char* str) {
	_lcd.print(str);
}

void LiquidCrystal::print(int number) {
	_lcd.print(number);
}

// --- Compatibility API function aliases ---

void LiquidCrystal::blink_on() {
	_lcd.blink_on();
}

void LiquidCrystal::blink_off() {
	_lcd.blink_off();
}

void LiquidCrystal::cursor_on() {
	_lcd.cursor_on();
}

void LiquidCrystal::cursor_off() {
	_lcd.cursor_off();
}

void LiquidCrystal::setBacklight(uint8_t new_val) {
	_lcd.setBacklight(new_val);
	_backlight_on = (new_val != 0);
}

void LiquidCrystal::load_custom_character(uint8_t char_num, uint8_t *rows) {
	_lcd.load_custom_character(char_num, rows);
}

void LiquidCrystal::printstr(const char c[]) {
	_lcd.printstr(c);
}
