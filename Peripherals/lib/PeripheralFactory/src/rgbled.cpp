#include "rgbled.h"
#include <Arduino.h>

RGBLED::RGBLED(uint8_t pin, uint16_t numPixels, neoPixelType type)
    : _strip(numPixels, pin, type), _brightness(255), _r(0), _g(0), _b(0) {}

void RGBLED::init() {
    _strip.begin();
    _strip.setBrightness(_brightness);
    _strip.show();
}

void RGBLED::update() {

}

void RGBLED::setColor(uint8_t r, uint8_t g, uint8_t b) {
    _r = r; _g = g; _b = b;
    _strip.setPixelColor(0, _strip.Color(r, g, b));
}

void RGBLED::setBrightness(uint8_t brightness) {
    _brightness = brightness;
    _strip.setBrightness(brightness);
}

void RGBLED::show() {
    _strip.show();
}
