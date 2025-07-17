#include "oled.h"

// Constructor: Initializes the member variables, including the _display object.
OLEDDisplay::OLEDDisplay(Width screenWidth, Height screenHeight, TwoWire *twi, RstPin resetPin)
    : _screenWidth(screenWidth.val), 
      _screenHeight(screenHeight.val),
      _twi(twi),
      _resetPin(resetPin.val),
      _display(screenWidth.val, screenHeight.val, twi, resetPin.val) {}

// init() is called by the PeripheralFactory. It initializes the actual display.
void OLEDDisplay::init() {
    // The I2C address is often 0x3C. We use that as a default.
    if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        // In a real scenario, you might want to handle this failure.
    }
    // Set up some default states
    _display.clearDisplay();
    _display.setTextColor(SSD1306_WHITE);
    _display.setTextSize(1);
    _display.setCursor(0,0);
    _display.display();
}

// --- Wrapper Methods ---
// The rest of the methods simply call the corresponding method on the _display object.

void OLEDDisplay::clear() {
    _display.clearDisplay();
}

void OLEDDisplay::show() {
    _display.display();
}

void OLEDDisplay::setCursor(int16_t x, int16_t y) {
    _display.setCursor(x, y);
}

void OLEDDisplay::setTextSize(uint8_t size) {
    _display.setTextSize(size);
}

void OLEDDisplay::setTextColor(uint16_t color) {
    _display.setTextColor(color);
}

void OLEDDisplay::print(const String &s) {
    _display.print(s);
}

void OLEDDisplay::print(const char* s) {
    _display.print(s);
}

void OLEDDisplay::print(char c) {
    _display.print(c);
}

void OLEDDisplay::print(int n, int base) {
    _display.print(n, base);
}

void OLEDDisplay::print(unsigned int n, int base) {
    _display.print(n, base);
}

void OLEDDisplay::print(long n, int base) {
    _display.print(n, base);
}

void OLEDDisplay::print(unsigned long n, int base) {
    _display.print(n, base);
}

void OLEDDisplay::print(double n, int digits) {
    _display.print(n, digits);
}

void OLEDDisplay::println(const String &s) {
    _display.println(s);
}

void OLEDDisplay::println(const char* s) {
    _display.println(s);
}

void OLEDDisplay::println(char c) {
    _display.println(c);
}

void OLEDDisplay::println(int n, int base) {
    _display.println(n, base);
}

void OLEDDisplay::println(unsigned int n, int base) {
    _display.println(n, base);
}

void OLEDDisplay::println(long n, int base) {
    _display.println(n, base);
}

void OLEDDisplay::println(unsigned long n, int base) {
    _display.println(n, base);
}

void OLEDDisplay::println(double n, int digits) {
    _display.println(n, digits);
}

void OLEDDisplay::println() {
    _display.println();
}

// getDisplay() returns a reference to the internal _display object.
Adafruit_SSD1306& OLEDDisplay::getDisplay() {
    return _display;
}
