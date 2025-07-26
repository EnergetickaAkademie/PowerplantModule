#ifndef RGBLED_H
#define RGBLED_H

#include "Peripheral.h"
#include <Adafruit_NeoPixel.h>

class RGBLED : public Peripheral {
public:
    RGBLED(uint8_t pin, uint16_t numPixels = 1, neoPixelType type = NEO_GRB + NEO_KHZ800);
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
