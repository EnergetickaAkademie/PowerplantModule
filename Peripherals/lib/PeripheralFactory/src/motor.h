#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "Peripheral.h"

/**
 * @brief A simple motor driver peripheral that controls a motor using two pins.
 * 
 * This class provides methods to initialize the motor, set its direction and speed,
 * and stop the motor. It uses PWM for speed control.
 * 
 * `Motor motor(uint8_t pinIA, uint8_t pinIB);`
 * @param pinIA The GPIO pin for the motor's input A.
 * @param pinIB The GPIO pin for the motor's input B.
 */
class Motor : public Peripheral {
public:
    Motor(PinA pinIA, PinB pinIB);
    Motor(uint8_t pinIA, uint8_t pinIB): Motor(PinA{pinIA}, PinB{pinIB}) {}

    void init() override; // Renamed from begin()
    void forward(int speed);
    void backward(int speed);
    void stop();

private:
    int _pinIA;
    int _pinIB;
};

#endif // MOTOR_H
