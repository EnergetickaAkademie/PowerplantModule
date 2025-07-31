#ifndef ENCODER_H
#define ENCODER_H

#include "Peripheral.h"
#include <AiEsp32RotaryEncoder.h>

class Encoder : public Peripheral {
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100, int16_t steps_per_click = 1,
    bool enable_speedup = false, unsigned int speedup_increment = 5, unsigned int speedup_interval = 75);
    ~Encoder();
    
    void update() override;

    int16_t getValue();
    void setValue(int16_t value);
    bool isButtonPressed();
    void setRange(int16_t minVal, int16_t maxVal);
	void enable();
	void disable();
    int16_t getUpperBound();
    int16_t getLowerBound();
    int16_t getStepsPerClick();
    
private:
    AiEsp32RotaryEncoder* _rotary;  // Pointer to instance-specific encoder
    int16_t _minVal;
    int16_t _maxVal;
    int16_t _stepsPerClick;
    uint8_t _pinA;  // Store pins for ISR registration
    uint8_t _pinB;
    uint8_t _pinSW;

    // Static method to handle ISR
    static void IRAM_ATTR readEncoderISR();
};
#endif