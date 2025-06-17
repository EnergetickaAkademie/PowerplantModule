#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "Peripheral.h"

class Motor : public Peripheral {
public:
    Motor(int pinIA, int pinIB);
    void init() override; // Renamed from begin()
    void forward(int speed);
    void backward(int speed);
    void stop();

private:
    int _pinIA;
    int _pinIB;
};

#endif // MOTOR_H
