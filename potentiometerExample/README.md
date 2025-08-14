# ESP32-S3 Logarithmic Potentiometer Reader

This PlatformIO project demonstrates how to read a logarithmic potentiometer (slider) connected to GPIO pin 12 of an ESP32-S3 DevKit board, with proper compensation for the logarithmic response curve.

## Hardware Setup

### Components Required:
- ESP32-S3-DevKitC-1 board
- **Logarithmic potentiometer** (any value, typically 10kΩ audio taper)
- Jumper wires

### Wiring:
```
Potentiometer:
- Pin 1 (Left)   → 3.3V (VCC)
- Pin 2 (Center) → GPIO 12 (ADC input)
- Pin 3 (Right)  → GND
```

## Software Features

- **ADC Resolution**: 12-bit (0-4095)
- **Voltage Range**: 3.3V (ESP32-S3 operating voltage)
- **Mapping Range**: 
  - Input: 10% to 90% of VCC (0.33V to 2.97V)
  - Output: 0 to 100
- **Serial Output**: Real-time display of raw ADC, voltage, and linearized values
- **Logarithmic Compensation**: Applies exponential mapping to compensate for the logarithmic taper of the potentiometer

## Code Explanation

The code performs the following operations:

1. **ADC Configuration**: Sets 12-bit resolution and appropriate attenuation
2. **Voltage Reading**: Converts raw ADC values to voltage
3. **Logarithmic Compensation**: Applies exponential mapping to linearize the logarithmic potentiometer response
4. **Range Mapping**: Maps the compensated values to 0-100 scale
5. **Serial Output**: Displays all values for monitoring

### Key Parameters:

```cpp
const int POTENTIOMETER_PIN = 12;      // ADC pin
const float VCC = 3.3;                 // Operating voltage
const float MIN_VOLTAGE = VCC * 0.1;   // 0.33V (maps to 0)
const float MAX_VOLTAGE = VCC * 0.9;   // 2.97V (maps to 100)
```

## Building and Uploading

### Prerequisites:
- PlatformIO installed
- ESP32-S3 board connected via USB

### Commands

```bash
# Build the project
pio run

# Build and upload
pio run --target upload

# Monitor serial output
pio device monitor
```

Or alternatively:
1. Build and upload the project using PlatformIO
2. Open the serial monitor at 115200 baud
3. Adjust the potentiometer and observe the output values

## Expected Output

```
ESP32-S3 Logarithmic Potentiometer Reader
=========================================
Pin: 12
Range: 0-100 (linearized from logarithmic potentiometer)
Min voltage: 0.33V
Max voltage: 2.97V
Note: Exponential compensation applied for log pot

Raw ADC: 1024 | Voltage: 0.825V | Linearized Value: 15%
Raw ADC: 2048 | Voltage: 1.650V | Linearized Value: 50%
Raw ADC: 3072 | Voltage: 2.475V | Linearized Value: 85%
...
```

## Configuration

The project is configured for ESP32-S3-DevKitC-1 in `platformio.ini`:

- **Platform**: espressif32
- **Board**: esp32-s3-devkitc-1
- **Framework**: Arduino
- **Monitor Speed**: 115200 baud

## Notes

- The potentiometer should be connected as a voltage divider
- **Logarithmic potentiometer**: Common in audio applications (audio taper)
- **Compensation applied**: Exponential mapping linearizes the logarithmic response
- Values below 10% VCC will read as 0
- Values above 90% VCC will read as 100
- ADC attenuation is set to 11dB for full 3.3V range
- Serial monitor baud rate: 115200
- The mapping ensures a 0-100 range using only the middle 80% of the potentiometer travel
- **Linearized output**: The compensated values should feel more linear when adjusting the potentiometer
