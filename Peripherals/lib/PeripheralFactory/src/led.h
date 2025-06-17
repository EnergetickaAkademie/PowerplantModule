#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include "Peripheral.h"

class LED : public Peripheral {
public:
    LED(int pin);
    void init() override; // Renamed from begin()
    void on();
    void off();
    void startBlink(unsigned long interval);
    void stopBlink(bool leaveOn = false);
    void update() override; // This already fits the Peripheral interface

private:
    int _pin;
    bool _blinking;
    unsigned long _blinkInterval;
    unsigned long _previousMillisBlink;
    int _ledStateBlink; // Stores the current state (HIGH or LOW) during blinking
};

#endif // LED_H
