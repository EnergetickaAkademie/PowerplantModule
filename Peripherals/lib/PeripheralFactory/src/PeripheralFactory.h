#ifndef PERIPHERAL_FACTORY_H
#define PERIPHERAL_FACTORY_H

#include "Peripheral.h"
// Include all concrete peripheral headers so the factory can create them.
#include "led.h"
#include "motor.h"
#include "oled.h"
#include "segment.h"
#include "encoder.h"

#define MAX_PERIPHERALS 10 

/**
 * @brief Manages and creates hardware peripherals.
 * 
 * This class uses a factory pattern to create, manage, initialize, and update
 * all hardware peripherals from a central point.
 */
class PeripheralFactory {
public:
    PeripheralFactory();
    ~PeripheralFactory(); // Destructor to clean up dynamically allocated peripherals

    // --- Factory Methods ---
    // Create methods for each peripheral type that return a typed pointer.
    LED* createLed(int pin);
    Motor* createMotor(int pinIA, int pinIB);
    OLEDDisplay* createOLED(uint8_t w, uint8_t h, TwoWire *twi, int8_t rst_pin = -1);
    Segment* createSegment(int dataPin, int clkPin, int csPin, int numDevices = 1);
    Encoder* createEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100, int16_t step = 1);

    /**
     * @brief Calls the init() method on all registered peripherals.
     */
    void init();

    /**
     * @brief Calls the update() method on all registered peripherals.
     */
    void update();

private:
    void add(Peripheral* peripheral); // Private, used internally by `create` methods

    Peripheral* _peripherals[MAX_PERIPHERALS];
    int _peripheralCount;
};

#endif // PERIPHERAL_FACTORY_H