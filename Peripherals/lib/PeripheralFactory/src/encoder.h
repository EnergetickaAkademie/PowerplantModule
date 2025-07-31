#ifndef ENCODER_H
#define ENCODER_H

#include "Peripheral.h"
#include <ESP32RotaryEncoder.h>

class Encoder : public Peripheral {
public:
    // Simplified constructor
    Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100);
    
    // The update method is no longer needed as the library is interrupt-driven
    void update() override {}

    int16_t getValue();
    void setValue(int16_t value);
    bool isButtonPressed();
    int16_t getUpperBound();
    void reverse();
    
    // Public method that can be called from the ISR helper
    void button_callback();

private:
    RotaryEncoder _rotary;
    int16_t _maxVal;
    bool _reversed = false;
    
    // Track the current value from the encoder callback
    volatile int16_t _currentValue = 0;
    
    // We need a flag for the button press, which will be set in a callback
    volatile bool _buttonWasPressed = false;
};

#endif // ENCODER_H