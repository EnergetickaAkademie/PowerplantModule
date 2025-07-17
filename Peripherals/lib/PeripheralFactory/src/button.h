#ifndef BUTTON_H
#define BUTTON_H

#include "Peripheral.h"
#include <Button2.h>
#include <functional>

typedef std::function<void(Button2&)> button_callback_t;

/**
 * @brief A button peripheral that uses the Button2 library for advanced button handling.
 * 
 * This class wraps the Button2 functionality to provide a simple interface for button events.
 * It supports click, double-click, long-press, and triple-click events.
 * 
 * `Button button(uint8_t pin);`
 * @param pin The GPIO pin number to which the button is connected.
 */
class Button : public Peripheral {
public:
	Button(Pin pin);
	Button(ButtonPin pin) : Button(Pin{pin.val}) {}
	Button(uint8_t pin) : Button(Pin{pin}) {}
	
	void init() override;
	void update() override;

	void setClickHandler(button_callback_t f);
	void setDoubleClickHandler(button_callback_t f);
	void setLongClickHandler(button_callback_t f);
	void setTripleClickHandler(button_callback_t f);

private:
	Button2 _button;
};

#endif // BUTTON_H
