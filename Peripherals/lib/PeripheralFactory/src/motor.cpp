#include "motor.h"

Motor::Motor(int pinIA, int pinIB) {
	_pinIA = pinIA;
	_pinIB = pinIB;
	
	pinMode(_pinIA, OUTPUT);
	pinMode(_pinIB, OUTPUT);
	stop(); 
}

void Motor::forward(int speed) {
	// Ensure speed is within ESP8266 PWM range 0-1023
	int pwmSpeed = constrain(speed, 0, 1023);
	analogWrite(_pinIA, pwmSpeed);
	analogWrite(_pinIB, 0);
}

void Motor::backward(int speed) {
	// Ensure speed is within ESP8266 PWM range 0-1023
	int pwmSpeed = constrain(speed, 0, 1023);
	analogWrite(_pinIA, 0);
	analogWrite(_pinIB, pwmSpeed);
}

void Motor::stop() {
	analogWrite(_pinIA, 0);
	analogWrite(_pinIB, 0);
}
