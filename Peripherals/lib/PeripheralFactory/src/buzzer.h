#ifndef BUZZER_H
#define BUZZER_H

#include "Peripheral.h"
#include <Arduino.h>

/**
 * @brief A simple buzzer peripheral that can produce a buzz sound for a specified duration.
 * 
 * This class provides methods to initialize the buzzer and control its buzzing behavior.
 * It uses a digital pin to turn the buzzer on and off.
 * 
 * `Buzzer buzzer(uint8_t pin);`
 * @param pin The GPIO pin number to which the buzzer is connected.
 */
class Buzzer : public Peripheral {
public:
	Buzzer(Pin pin);

	Buzzer(uint8_t pin) : Buzzer(Pin{pin}) {}

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
