#ifndef BUTTON_H
#define BUTTON_H

#include "Peripheral.h"
#include <Button2.h>
#include <functional>

typedef std::function<void(Button2&)> button_callback_t;

class Button : public Peripheral {
public:
	Button(uint8_t pin);
	
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
