#ifndef PERIPHERAL_FACTORY_H
#define PERIPHERAL_FACTORY_H

#include "Peripheral.h"
// Include all concrete peripheral headers so the factory can create them.
#include "led.h"
#include "motor.h"
#include "oled.h"
#include "encoder.h"
#include "rgbled.h"
#include "buzzer.h"
#include "shift_register_chain.h"
#include "segment_display.h"
#include "bargraph.h"
#include "liquid_crystal.h"
#include "button.h"
#include "ledbutton.h"
#include <utility>

#define MAX_PERIPHERALS 100 

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

    template <typename T, typename... Args>
    T* create(Args&&... args);

    // --- Legacy Factory Methods (deprecated, use create<T>() instead) ---
    LED* createLed(int pin) { return create<LED>(pin); }
    Motor* createMotor(int pinIA, int pinIB) { return create<Motor>(pinIA, pinIB); }
    OLEDDisplay* createOLED(uint8_t w, uint8_t h, TwoWire *twi, int8_t rst_pin = -1) { return create<OLEDDisplay>(w, h, twi, rst_pin); }
    Encoder* createEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal = 0, int16_t maxVal = 100, int16_t step = 1) { return create<Encoder>(pinA, pinB, pinSW, minVal, maxVal, step); }
    RGBLED* createRGBLED(uint8_t pin, uint16_t numPixels = 1, neoPixelType type = NEO_GRB + NEO_KHZ800) { return create<RGBLED>(pin, numPixels, type); }
    Buzzer* createBuzzer(uint8_t pin) { return create<Buzzer>(pin); }
    ShiftRegisterChain* createShiftRegisterChain(uint8_t latchPin, uint8_t dataPin, uint8_t clockPin) { return create<ShiftRegisterChain>(latchPin, dataPin, clockPin); }
    LiquidCrystal* createLiquidCrystal(uint8_t address, uint8_t cols, uint8_t rows) { return create<LiquidCrystal>(address, cols, rows); }
    Button* createButton(uint8_t pin) { return create<Button>(pin); }
    LEDButton* createLEDButton(uint8_t buttonPin, uint8_t ledPin) { return create<LEDButton>(buttonPin, ledPin); }

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


template <typename T, typename... Args>
T* PeripheralFactory::create(Args&&... args) {
    T* peripheral = new T(std::forward<Args>(args)...);
    
    add(peripheral);
    
    return peripheral;
}


template <>
inline Bargraph* PeripheralFactory::create<Bargraph>(uint8_t& numLeds) {
    // Create the bargraph but don't add it to the factory's peripheral list
    // It will be managed by the ShiftRegisterChain instead
    return new Bargraph(numLeds);
}

#endif // PERIPHERAL_FACTORY_H


    /**
     * @brief Creates and registers a peripheral of a specific type.
     * 
     * This is a template function that can create any peripheral that inherits
     * from the Peripheral base class. It forwards all arguments directly to the
     * peripheral's constructor, preserving perfect type safety.
     * 
     * All peripherals created through this factory are automatically:
     * - Added to the factory's peripheral list for unified management
     * - Initialized when initAll() is called
     * - Updated when updateAll() is called
     * 
     * Note: Shift register devices (Bargraph, SegmentDisplay) are created and
     * managed by the factory like any other peripheral, but they must also be
     * manually added to a ShiftRegisterChain using chain->addDevice(device) to
     * output their data to the hardware shift registers.
     * 
     * @tparam T The type of peripheral to create (e.g., LED, Motor, LEDButton).
     * @tparam Args The types of the constructor arguments.
     * @param args The arguments to pass to the peripheral's constructor.
     * @return A strongly-typed pointer to the newly created peripheral (e.g., LED*).
     * 
     * Examples:
     *   LED* led = factory.create<LED>(5);
     *   Motor* motor = factory.create<Motor>(1, 2);
     *   LEDButton* btn = factory.create<LEDButton>(6, 5);
     *   Encoder* enc = factory.create<Encoder>(10, 11, 12, 0, 255, 5);
     *   
     *   // Shift register devices (all peripherals are managed by factory):
     *   ShiftRegisterChain* chain = factory.create<ShiftRegisterChain>(8, 9, 10);
     *   Bargraph* bar = factory.create<Bargraph>(16);
     *   SegmentDisplay* seg = factory.create<SegmentDisplay>(4);
     *   chain->addDevice(bar);  // Add to chain for hardware output
     *   chain->addDevice(seg);  // Add to chain for hardware output
     */