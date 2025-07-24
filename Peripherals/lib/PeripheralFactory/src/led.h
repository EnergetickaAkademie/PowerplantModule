#ifndef LED_H
#define LED_H

#include "Peripheral.h"
#include <Arduino.h>

class LED : public Peripheral {
public:
    LED(int pin);

    void init() override;
    void update() override;

    void on();
    void off();
    void setState(bool state);
    void startBlink(unsigned long interval);
    void stopBlink();
    
    void setBrightness(uint8_t brightness);

private:
    int _pin;
    bool _isBlinking;
    bool _ledState;
    unsigned long _blinkInterval;
    unsigned long _lastToggleTime;
    
    uint8_t _brightness; 
};

#endif // LED_H