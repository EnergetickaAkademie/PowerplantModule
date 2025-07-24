#ifndef LED_BUTTON_H
#define LED_BUTTON_H

#include "Peripheral.h"
#include "led.h" // We will use our LED wrapper
#include <Button2.h> // We use Button2 directly for more control
#include <vector>

// Defines the automatic behavior of the LED when the button is used.
enum class LEDButtonMode {
	MANUAL, // User controls the LED directly via getLED()
	FOLLOW, // LED is on only when the button is physically pressed.
	TOGGLE  // LED toggles on/off with each button click.
};

enum class UpdateFunction {
	PRESS,
	RELEASE,
	TOGGLE
};

/**
 * @brief A button that controls an LED, with various modes of operation.
 * 
 * This class combines a Button2 instance for button handling and an LED instance for visual feedback.
 * It supports different modes like FOLLOW, TOGGLE, and MANUAL control of the LED, with update functions that can be added for TOGGLE, PRESS, and RELEASE events.
 * 
 * `LEDButton ledButton(uint8_t buttonPin, uint8_t ledPin);`
 * @param buttonPin The GPIO pin number for the button.
 * @param ledPin The GPIO pin number for the LED.
 */
class LEDButton : public Peripheral {
public:
	LEDButton(uint8_t buttonPin,uint8_t ledPin);
	~LEDButton();

	void init() override;
	void update() override;

	// --- Configuration ---
	void setMode(LEDButtonMode mode);

	// --- Manual Control ---
	LED* getLED();       // Get the raw LED object for manual control (e.g., ledButton->getLED()->on())
	Button2* getButton(); // Get the raw Button2 object to attach your own custom handlers

	bool getToggleState();

	void setToggleState(bool state);

	std::vector<std::function<void()>> toggleFuncs;
	std::vector<std::function<void()>> pressFuncs;
	std::vector<std::function<void()>> releaseFuncs;

	void addUpdateFunction(std::function<void()> func, UpdateFunction type);

	void clearUpdateFunctions();

	void clearUpdateFunctions(UpdateFunction type);

private:
	// These are now regular, non-static member functions.
	void _followPressed(Button2& b);
	void _followReleased(Button2& b);
	void _toggleClicked(Button2& b);

	LED* _led;
	Button2 _button;
	LEDButtonMode _mode;
	bool _toggleState;
};

#endif // LED_BUTTON_H
