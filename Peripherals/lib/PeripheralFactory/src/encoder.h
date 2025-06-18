#ifndef ENCODER_H
#define ENCODER_H

#include "Peripheral.h"
#include <ESPRotary.h>

class Encoder : public Peripheral {
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100, int16_t steps_per_click = 1);
    
    void init() override;
    void update() override;

    int16_t getValue();
    bool isButtonPressed();
    void setValue(int16_t value);

private:
    ESPRotary _rotary;
    uint8_t _pinSW;

    // This will store the position updated by the callback
    volatile int16_t _currentPosition;

    // Button debouncing variables
    uint8_t _lastButtonState;
    unsigned long _lastDebounceTime;
    bool _buttonWasPressed;

    // Static members to route the C-style callback to our C++ object instance
    static void IRAM_ATTR _static_rotation_callback(ESPRotary &r);
    static Encoder* _instance;
};

#endif // ENCODER_H