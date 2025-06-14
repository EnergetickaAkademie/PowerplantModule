# OneWire ESP8266 Master-Slave Communication

A simple and reliable OneWire communication system for ESP8266 devices using the proven OneWire library. The master sends heartbeats with device IDs, and slaves respond by blinking their LED a number of times equal to their assigned ID.

## 🎯 **Core Functionality**

- **Master**: Cycles through slave IDs (1-5), sending heartbeats every 3 seconds
- **Slaves**: Listen for their specific ID and blink LED accordingly
  - Slave ID 1 → Blinks 1 time
  - Slave ID 2 → Blinks 2 times  
  - Slave ID 3 → Blinks 3 times
  - And so on...

## 🚀 **Quick Start**

### 1. Hardware Setup
- Connect all ESP8266 devices to the same OneWire bus (GPIO2)
- Add one 4.7kΩ pull-up resistor between bus and 3.3V
- Connect common ground between all devices

### 2. Software Configuration

**Master Device:**
```cpp
#define DEVICE_MODE_MASTER true
#define SLAVE_ID 1               // Not used for master
```

**Slave Device (ID 2):**
```cpp
#define DEVICE_MODE_MASTER false
#define SLAVE_ID 2               // This device blinks 2 times
```

### 3. Upload and Test
1. Flash one ESP8266 as master
2. Flash other ESP8266s as slaves with unique IDs
3. Connect to OneWire bus and power on
4. Watch slaves blink when their ID is called!

## 📋 **Dependencies**

The project automatically installs:
- **OneWire** by Paul Stoffregen (v2.3.7) - Handles all low-level OneWire communication

## 🔧 **Detailed Setup**

See `SETUP_GUIDE.md` for complete step-by-step instructions including:
- Wiring diagrams
- Device configuration examples  
- Expected serial monitor output
- Troubleshooting guide

## Hardware Setup

### Pin Configuration
- **OneWire Bus**: GPIO2 (D4 on D1 Mini)
- **LED**: Built-in LED (GPIO2 on most ESP8266 boards)

### Wiring
1. Connect all devices to the same OneWire bus (GPIO2)
2. Use a 4.7kΩ pull-up resistor between the OneWire bus and VCC
3. Connect GND of all devices together
4. Ensure proper power supply for all devices

```
Master Device    Slave Device 1    DS18B20 Sensor
     |                 |                 |
     +-------[4.7kΩ]---+                 |
     |                 |                 |
   GPIO2             GPIO2             DQ
     |                 |                 |
     +-----------------+-----------------+  (OneWire Bus)
     |                 |                 |
    GND               GND               GND
     |                 |                 |
     +-----------------+-----------------+  (Common Ground)
```

## Software Configuration

### Setting Device Mode

In `src/main.cpp`, change the following line to configure the device:

```cpp
#define DEVICE_MODE_MASTER true   // For master device
#define DEVICE_MODE_MASTER false  // For slave device (simplified implementation)
```

### Library Installation

The `platformio.ini` file automatically includes the OneWire library:

```ini
[env:d1_mini]
platform = espressif8266
board = d1_mini
framework = arduino
lib_deps = 
    paulstoffregen/OneWire@^2.3.7
```

## Protocol Details

### OneWire Library Advantages
- **Proven Timing**: Handles all critical OneWire timing automatically
- **CRC Checking**: Built-in CRC8 calculation and verification
- **Device Search**: Robust device enumeration and addressing
- **Cross-Platform**: Works on multiple Arduino-compatible platforms
- **Well-Tested**: Used in thousands of projects worldwide

### Custom Protocol Commands
- **0xAA (HEARTBEAT_COMMAND)**: Master heartbeat signal
- **0xBB (TEMP_REQUEST_COMMAND)**: Temperature request (for DS18B20 examples)

### Device ROM Structure
Each device has a unique 8-byte ROM address:
- **Byte 0**: Family code (0x28 for our custom devices)
- **Bytes 1-6**: Unique serial number (based on ESP chip ID)
- **Byte 7**: CRC8 checksum

## Operation

### Master Device
1. Automatically searches for devices on startup
2. Sends heartbeat signals every 2 seconds using `oneWire.reset()` and `oneWire.write()`
3. Uses Skip ROM command (0xCC) to address all devices simultaneously
4. Periodically re-scans for new devices (every 10 seconds)
5. Provides detailed serial output about discovered devices

### Slave Device (Simplified)
1. **Note**: True OneWire slave implementation requires dedicated hardware
2. This implementation provides a simplified demonstration
3. Monitors the bus for reset pulses and attempts to read commands
4. Blinks LED when heartbeat is detected
5. For production use, consider using dedicated OneWire slave controllers

## LED Patterns

### Master
- **Single quick blink**: Heartbeat sent successfully
- **No blink**: No devices found on bus

### Slave (Simplified)
- **Three quick blinks**: Heartbeat received from master
- **No blink**: No communication detected

## Serial Monitor Output

Enable Serial monitoring at 115200 baud to see:
- Device initialization and library version
- Automatic device discovery with ROM addresses
- CRC verification results
- Heartbeat transmission confirmations
- Real-time bus activity monitoring

Example output:
```
=== OneWire Heartbeat Protocol ===
Using OneWire library v2.3.7
Device ROM: 28 AB CD EF 12 34 56 78

=== Searching for devices ===
Device #1 ROM: 28 12 34 56 78 9A BC DE (CRC OK)
Found 1 devices

=== Sending Heartbeat ===
Heartbeat sent to all devices
```

## Examples

### Basic Heartbeat
The main implementation in `src/main.cpp` provides basic master-slave heartbeat functionality.

### DS18B20 Temperature Sensors
See `examples/with_ds18b20_example.cpp` for an advanced example that:
- Combines heartbeat with real OneWire temperature sensors
- Demonstrates device enumeration and addressing
- Shows temperature reading alongside custom commands
- Provides a practical test setup with real OneWire slaves

### Different Configurations
See `examples/configurations.cpp` for various setup examples including:
- Fast heartbeat configurations
- Custom pin assignments
- Multiple bus configurations

## Building and Uploading

1. Open the project in PlatformIO
2. The OneWire library will be automatically downloaded and installed
3. Configure device mode in `main.cpp`
4. Build the project: `pio run`
5. Upload to device: `pio run --target upload`
6. Monitor serial output: `pio device monitor`

## Testing with Real OneWire Devices

### Using DS18B20 Temperature Sensors
1. Connect DS18B20 sensors to the OneWire bus
2. Add DallasTemperature library to `platformio.ini`:
   ```ini
   lib_deps = 
       paulstoffregen/OneWire@^2.3.7
       milesburton/DallasTemperature@^3.11.0
   ```
3. Use the example in `examples/with_ds18b20_example.cpp`
4. Observe both heartbeat and temperature readings

### Device Search Output
The master will automatically discover and list all OneWire devices:
```
=== Searching for devices ===
Device #1 ROM: 28 FF 12 34 56 78 9A BC (CRC OK)  // DS18B20 sensor
Device #2 ROM: 28 AA BB CC DD EE FF 11 (CRC OK)  // Custom device
Found 2 devices
```

## Troubleshooting

### Library Issues
- Ensure PlatformIO has internet access to download libraries
- Check `platformio.ini` syntax for library dependencies
- Use `pio lib list` to verify installed libraries

### No Communication
- Verify OneWire library is properly installed
- Check wiring connections and pull-up resistor
- Monitor serial output for device discovery results
- Test with known OneWire devices (like DS18B20)

### CRC Errors
- Check for electrical noise or interference
- Verify power supply stability
- Ensure proper grounding
- Check cable length (keep under 100m)

## Advantages of Library-Based Approach

### Reliability
- **Proven Code**: OneWire library has been tested in millions of deployments
- **Precise Timing**: Hardware-optimized timing routines
- **Error Handling**: Built-in error detection and recovery

### Compatibility
- **Standard Compliance**: Follows Maxim OneWire specifications exactly
- **Device Support**: Works with all standard OneWire devices
- **Platform Support**: Compatible with ESP8266, ESP32, Arduino, etc.

### Maintainability
- **Less Code**: Eliminates need for low-level timing implementation
- **Bug-Free**: Avoids common timing and electrical issues
- **Updates**: Benefits from library updates and improvements

## Extending the Protocol

### Adding New Commands
```cpp
#define CUSTOM_COMMAND 0xDD
// Send custom command
oneWire.reset();
oneWire.write(0xCC);  // Skip ROM
oneWire.write(CUSTOM_COMMAND);
```

### Device-Specific Communication
```cpp
// Address specific device
uint8_t deviceAddr[8] = {0x28, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE};
oneWire.reset();
oneWire.select(deviceAddr);
oneWire.write(HEARTBEAT_COMMAND);
```

### Reading Device Data
```cpp
// Read response from device
oneWire.reset();
oneWire.write(0xCC);  // Skip ROM
oneWire.write(READ_COMMAND);
uint8_t response = oneWire.read();
```

## Performance Notes

- **Timing Accuracy**: OneWire library provides microsecond-accurate timing
- **Device Limit**: Supports up to 100+ devices per bus (hardware dependent)
- **Speed**: Standard OneWire speed (~16.3 kbps) and overdrive speed (~142 kbps)
- **Reliability**: Built-in collision detection and error recovery

## Compatibility

- **Platform**: ESP8266, ESP32, Arduino Uno/Nano/Mega, etc.
- **Framework**: Arduino
- **IDE**: PlatformIO, Arduino IDE
- **Library**: OneWire v2.3.7+ by Paul Stoffregen
- **Devices**: Compatible with all standard OneWire devices
