#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include "value_types.h"

/**
 * @brief Abstract base class for all hardware peripherals.
 * 
 * This class defines a common interface that all peripheral drivers must implement.
 * It ensures that each peripheral has an initialization method and an optional update method
 * for non-blocking operations.
 */
class Peripheral {
public:
    /**
     * @brief Virtual destructor.
     * Ensures that derived class destructors are called correctly.
     */
    virtual ~Peripheral() {}

    /**
     * @brief Pure virtual initialization function.
     * 
     * This function must be implemented by all derived peripheral classes.
     * It should contain the setup code for the hardware, like setting pin modes.
     */
    virtual void init() = 0;

    /**
     * @brief Virtual update function for non-blocking tasks.
     * 
     * This function can be overridden by derived classes that need to perform
     * periodic tasks, such as blinking an LED without using delay().
     * It should be called repeatedly in the main loop.
     */
    virtual void update() {}
};

#endif // PERIPHERAL_H
