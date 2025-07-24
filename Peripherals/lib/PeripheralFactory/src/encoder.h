#ifndef ENCODER_H
#define ENCODER_H

#include "Peripheral.h"
#include <ESPRotary.h>
#include <Button2.h>

class Encoder : public Peripheral {
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100, int16_t steps_per_click = 1);
    
    void init() override;
    void update() override;

    int16_t getValue();
    void setValue(int16_t value);
    bool isButtonPressed();

private:
    ESPRotary _rotary;
    Button2 _button;
    volatile int16_t _currentPosition;
    volatile bool _buttonPressed;

    // Static members to route the C-style callback to our C++ object instance
    static Encoder* _instance;
    static void rotation_callback(ESPRotary &r);
    static void button_callback(Button2& b);
};

#endif // ENCODER_H