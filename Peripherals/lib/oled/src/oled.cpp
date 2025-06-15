#include "oled.h"

OLEDDisplay::OLEDDisplay(uint8_t screenWidth, uint8_t screenHeight, TwoWire *twi, int8_t resetPin)
    : _screenWidth(screenWidth), 
      _screenHeight(screenHeight),
      _twi(twi),
      _resetPin(resetPin),
      _display(screenWidth, screenHeight, twi, resetPin) {}

bool OLEDDisplay::begin(uint8_t i2cAddress, bool switchvcc) {
    if (!_display.begin(switchvcc ? SSD1306_SWITCHCAPVCC : 0, i2cAddress)) {
        return false;
    }
    return true;
}

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

Adafruit_SSD1306& OLEDDisplay::getDisplay() {
    return _display;
}
