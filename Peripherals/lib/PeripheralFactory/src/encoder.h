#ifndef ENCODER_H
#define ENCODER_H

#include "Peripheral.h"
#include <ESPRotary.h>
#include <Button2.h>
#include <map>

class Encoder : public Peripheral {
public:
	Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100, int16_t steps_per_click = 1,
	bool enable_speedup = false, unsigned int speedup_increment = 5, unsigned int speedup_interval = 75);
	~Encoder();
	
	void update() override;

	int16_t getValue();
	void setValue(int16_t value);
	bool isButtonPressed();
	void setRange(int16_t minVal, int16_t maxVal);
	int16_t getUpperBound();
	int16_t getLowerBound();

	void reverse();

private:
	ESPRotary _rotary;
	Button2 _button;
	volatile int16_t _currentPosition;
	volatile bool _buttonPressed;

	// Static members to route the C-style callback to our C++ object instance
	static std::map<ESPRotary*, Encoder*> _rotary_map;
	static std::map<Button2*, Encoder*> _button_map;
	static void rotation_callback(ESPRotary &r);
	static void button_callback(Button2& b);
};

#endif // ENCODER_H