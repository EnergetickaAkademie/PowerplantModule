#ifndef ENCODER_H
#define ENCODER_H

#include "Peripheral.h"
#include <ESPRotary.h>
#include <Button2.h>

/**
 * @brief A rotary encoder peripheral that combines an ESPRotary for rotation detection
 * and a Button2 for button press handling.
 * 
 * This class provides methods to initialize the encoder, update its state, and retrieve
 * the current position and button press status.
 * 
 * `Encoder encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100, int16_t steps_per_click = 1);`
 * @param pinA The GPIO pin for the encoder's A signal.
 * @param pinB The GPIO pin for the encoder's B signal.
 * @param pinSW The GPIO pin for the encoder's switch button.
 * @param minVal The minimum value for the encoder position. Default is 0.
 * @param maxVal The maximum value for the encoder position. Default is 100.
 * @param steps_per_click The number of steps per click for the encoder. Default is 1.
 */
class Encoder : public Peripheral {
public:
	Encoder(PinA pinA, PinB pinB, PinSW pinSW, MinVal minVal = {0}, MaxVal maxVal = {100}, Step steps_per_click = {1});

	Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100, int16_t steps_per_click = 1)
		: Encoder(PinA{pinA}, PinB{pinB}, PinSW{pinSW}, MinVal{minVal}, MaxVal{maxVal}, Step{steps_per_click}) {}
	
	void init() override;
	void update() override;

	int16_t getValue();
	void setValue(int16_t value);
	bool isButtonPressed();

private:
	ESPRotary _rotary;
	Button2 _button;
	volatile int16_t _currentPosition;
	volatile bool _buttonPressed;

	// Static members to route the C-style callback to our C++ object instance
	static Encoder* _instance;
	static void rotation_callback(ESPRotary &r);
	static void button_callback(Button2& b);
};

#endif // ENCODER_H