#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "Peripheral.h"

/**
 * @brief A wrapper class for the Adafruit SSD1306 OLED display.
 * 
 * This class provides a simple interface to control an OLED display using the Adafruit SSD1306 library.
 * It inherits from Peripheral to integrate with the PeripheralFactory framework.
 * 
 * `OLEDDisplay oled(screenWidth, screenHeight, &Wire, resetPin);`
 * @param screenWidth The width of the OLED display in pixels.
 * @param screenHeight The height of the OLED display in pixels.
 * @param twi The TwoWire instance for I2C communication (default is &Wire).
 * @param resetPin The GPIO pin used for resetting the display (default is -1, meaning no reset).
 */
class OLEDDisplay : public Peripheral {
public:
    OLEDDisplay(Width screenWidth, Height screenHeight, TwoWire *twi = &Wire, RstPin resetPin = {-1});
    OLEDDisplay(uint8_t screenWidth, uint8_t screenHeight, TwoWire *twi = &Wire, int8_t resetPin = -1)
        : OLEDDisplay(Width{screenWidth}, Height{screenHeight}, twi, RstPin{resetPin}) {}

    void init() override; // From Peripheral interface

    // Public interface mirroring Adafruit's library
    void clear();
    void show();
    void setCursor(int16_t x, int16_t y);
    void setTextSize(uint8_t size);
    void setTextColor(uint16_t color);
    
    // Print methods
    void print(const String &s);
    void print(const char* s);
    void print(char c);
    void print(int n, int base = DEC);
    void print(unsigned int n, int base = DEC);
    void print(long n, int base = DEC);
    void print(unsigned long n, int base = DEC);
    void print(double n, int digits = 2);
    void println(const String &s);
    void println(const char* s);
    void println(char c);
    void println(int n, int base = DEC);
    void println(unsigned int n, int base = DEC);
    void println(long n, int base = DEC);
    void println(unsigned long n, int base = DEC);
    void println(double n, int digits = 2);
    void println();
    
    Adafruit_SSD1306& getDisplay(); // To allow direct access if needed

private:
    Adafruit_SSD1306 _display;
    uint8_t _screenWidth;
    uint8_t _screenHeight;
    int8_t _resetPin;
    TwoWire *_twi;
};

#endif // OLED_DISPLAY_H
