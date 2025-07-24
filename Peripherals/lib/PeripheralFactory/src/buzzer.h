#ifndef BUZZER_H
#define BUZZER_H

#include "Peripheral.h"
#include <Arduino.h>

class Buzzer : public Peripheral {
public:
    Buzzer(uint8_t pin);

    void init() override;
    void update() override;

    // Starts a buzz for a specific duration in milliseconds
    void buzz(unsigned long duration_ms);

private:
    uint8_t _pin;
    bool _isBuzzing;
    unsigned long _startTime;
    unsigned long _duration;
};

#endif // BUZZER_H
