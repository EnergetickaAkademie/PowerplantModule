#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor {
public:
    Motor(int pinIA, int pinIB);
    void begin();
    void forward(int speed);
    void backward(int speed);
    void stop();

private:
    int _pinIA;
    int _pinIB;
};

#endif // MOTOR_H
