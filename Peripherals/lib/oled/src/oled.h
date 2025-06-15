#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

class OLEDDisplay {
public:
    OLEDDisplay(uint8_t screenWidth, uint8_t screenHeight, TwoWire *twi = &Wire, int8_t resetPin = -1);

    bool begin(uint8_t i2cAddress = 0x3C, bool switchvcc = SSD1306_SWITCHCAPVCC);
    void clear();
    void show(); // Renamed from display() to avoid conflict with Adafruit_SSD1306::display
    void setCursor(int16_t x, int16_t y);
    void setTextSize(uint8_t size);
    void setTextColor(uint16_t color);
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
