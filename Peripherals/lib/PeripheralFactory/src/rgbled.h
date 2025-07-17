#ifndef RGBLED_H
#define RGBLED_H

#include "Peripheral.h"
#include <Adafruit_NeoPixel.h>

/**
 * @brief A peripheral for controlling RGB LEDs using the Adafruit NeoPixel library.
 * 
 * This class provides methods to initialize the RGB LED strip, set colors, brightness,
 * and update the display. It uses a single pin for data and supports multiple pixels.
 * 
 * `RGBLED rgbled(uint8_t pin, uint16_t numPixels = 1, neoPixelType type = NEO_GRB + NEO_KHZ800);`
 * @param pin The GPIO pin number to which the LED strip is connected.
 * @param numPixels The number of pixels in the strip (default is 1).
 * @param type The type of NeoPixel (default is NEO_GRB + NEO_KHZ800).
 */
class RGBLED : public Peripheral {
public:
    RGBLED(Pin pin, NumPixels numPixels = {1}, neoPixelType type = NEO_GRB + NEO_KHZ800);
    RGBLED(uint8_t pin, uint16_t numPixels = 1, neoPixelType type = NEO_GRB + NEO_KHZ800): RGBLED(Pin{pin}, NumPixels{numPixels}, type) {}

    void init() override;
    void update() override;
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setBrightness(uint8_t brightness);
    void show();
private:
    Adafruit_NeoPixel _strip;
    uint8_t _brightness;
    uint8_t _r, _g, _b;
};

#endif // RGBLED_H
