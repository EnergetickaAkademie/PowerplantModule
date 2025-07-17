#ifndef VALUE_TYPES_H
#define VALUE_TYPES_H

#include <stdint.h>

struct Pin { uint8_t val; };

//shift registers
struct LatchPin { uint8_t val; };
struct DataPin { uint8_t val; };
struct ClockPin { uint8_t val; };

struct ButtonPin { uint8_t val; };

struct LedPin { uint8_t val; };

// I2C pins
struct SdaPin { uint8_t val; };
struct SclPin { uint8_t val; };
struct I2cAddress { uint8_t val; };

// SPI pins
struct CsPin { uint8_t val; };
struct MosiPin { uint8_t val; };
struct MisoPin { uint8_t val; };
struct SckPin { uint8_t val; };
struct RstPin { int8_t val; };

//encoder pins
struct PinA { uint8_t val; };
struct PinB { uint8_t val; };
struct PinSW { uint8_t val; };

//values

struct NumLeds { uint8_t val; };
struct NumDigits { uint8_t val; };
struct MaxVal { int16_t val; };
struct MinVal { int16_t val; };
struct Step { int16_t val; };
struct NumCols { uint8_t val; };
struct NumRows { uint8_t val; };
struct Width { uint8_t val; };
struct Height { uint8_t val; };
struct NumPixels { uint16_t val; };

#endif // VALUE_TYPES_H