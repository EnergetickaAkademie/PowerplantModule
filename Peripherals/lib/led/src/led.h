#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led {
public:
    Led(int pin);
    void begin();
    void on();
    void off();
    void startBlink(unsigned long interval);
    void stopBlink(bool leaveOn = false);
    void update(); // Call this in your main loop

private:
    int _pin;
    bool _blinking;
    unsigned long _blinkInterval;
    unsigned long _previousMillisBlink;
    int _ledStateBlink; // Stores the current state (HIGH or LOW) during blinking
};

#endif // LED_H
