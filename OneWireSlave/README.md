# StarWire ESP8266 PowerPlant Module Slave

A comprehensive powerplant module system using StarWire protocol for robust communication with master devices. Each slave represents a different type of powerplant with unique control commands and visual feedback.

## 🎯 **Core Functionality**

- **Multi-Powerplant Support**: 8 different powerplant types with unique behaviors
- **StarWire Slave**: Responds to control commands from master devices
- **Visual Feedback**: RGB LEDs and atomizers for different powerplant states
- **WebSerial Debug**: Remote monitoring via web interface
- **OTA Updates**: Wireless firmware updates
- **Configurable IDs**: Support for multiple slaves per powerplant type

## 🏭 **Powerplant Types & Control Commands**

### 1. **Photovoltaic (TYPE_PHOTOVOLTAIC = 1)**
- **Hardware**: RGB LED on D2, Solar panel sensor on A0
- **Behavior**: Brightness controlled by light sensor, color changes based on mode
- **Commands**:
  - `0x03` (BAT_IDLE) - **Idle Mode (Green)**: Shifts to orange when light is low
  - `0x01` (CMD_ON) - **Half Power Mode (Orange)**: Shifts to red when light is low  
  - `0x02` (CMD_OFF) - **Night Mode (Red)**: No reaction to light, stays red

### 2. **Wind (TYPE_WIND = 2)**
- **Hardware**: Single wire motor controller on D2
- **Behavior**: Motor runs at 50 power level when ON, stops when OFF, includes speedup functionality
- **Commands**:
  - `0x01` (CMD_ON) - Turn motor ON (50 power level with 2.5x speedup for 1000ms)
  - `0x02` (CMD_OFF) - Turn motor OFF

### 3. **Nuclear (TYPE_NUCLEAR = 3)**
- **Hardware**: Atomizer on D2
- **Behavior**: Simple ON/OFF control with mist effect
- **Commands**:
  - `0x01` (CMD_ON) - Turn atomizer ON
  - `0x02` (CMD_OFF) - Turn atomizer OFF

### 4. **Gas (TYPE_GAS = 4)**
- **Hardware**: RGB LED on D2
- **Behavior**: 5-level production control with color and brightness indication
- **Commands**:
  - `0x06` (GAS_LEVEL_1) - **Level 1**: Green, Low production (Brightness: 64)
  - `0x07` (GAS_LEVEL_2) - **Level 2**: Yellow-Green, Low-Medium production (Brightness: 96)
  - `0x08` (GAS_LEVEL_3) - **Level 3**: Blue, Medium production (Brightness: 128)
  - `0x09` (GAS_LEVEL_4) - **Level 4**: Orange, Medium-High production (Brightness: 192)
  - `0x0A` (GAS_LEVEL_5) - **Level 5**: Red, High production (Brightness: 255)
  - `0x02` (CMD_OFF) - Turn OFF

### 5. **Hydro (TYPE_HYDRO = 5)**
- **Hardware**: Single wire motor controller on D5
- **Behavior**: Motor runs at 100% speed when ON, stops when OFF
- **Commands**:
  - `0x01` (CMD_ON) - Turn motor ON (100% speed)
  - `0x02` (CMD_OFF) - Turn motor OFF

### 6. **Hydro Storage (TYPE_HYDRO_STORAGE = 6)**
- **Hardware**: RGB LED on D2
- **Behavior**: 5-level storage indication with color and brightness showing storage level
- **Commands**:
  - `0x0B` (HYDRO_STORAGE_LEVEL_1) - **100% Full**: Green (Discharging)
  - `0x0C` (HYDRO_STORAGE_LEVEL_2) - **75% Full**: Light Green  
  - `0x0D` (HYDRO_STORAGE_LEVEL_3) - **50% Full**: Orange (Idle)
  - `0x0E` (HYDRO_STORAGE_LEVEL_4) - **25% Full**: Light Red
  - `0x0F` (HYDRO_STORAGE_LEVEL_5) - **0% Empty**: Red (Charging)
  - `0x02` (CMD_OFF) - Turn OFF

### 7. **Coal (TYPE_COAL = 7)**
- **Hardware**: Atomizer on D2
- **Behavior**: Simple ON/OFF control with mist effect
- **Commands**:
  - `0x01` (CMD_ON) - Turn atomizer ON
  - `0x02` (CMD_OFF) - Turn atomizer OFF

### 8. **Battery (TYPE_BATTERY = 8)**
- **Hardware**: RGB LED on D2
- **Behavior**: 3-state battery status indication
- **Commands**:
  - `0x03` (BAT_IDLE) - **Idle State**: Orange color
  - `0x04` (BAT_CHARGING) - **Charging State**: Red color
  - `0x05` (BAT_DISCHARGE) - **Discharging State**: Green color

## 🚀 **Quick Start**

### 1. Hardware Setup
- Connect ESP8266 D1 (GPIO5) to StarWire DATA line
- Connect ESP8266 D7 (GPIO13) to StarWire CLK line
- Add pull-up resistors as needed for communication bus
- Connect RGB LED or atomizer to D2 based on powerplant type
- For photovoltaic: Connect solar panel sensor to A0

### 2. Configuration
Generated configurations are available in `platformio.ini` for slaves 1-9:
```ini
# Example: Coal powerplant
[env:slave_1]
build_flags = 
    -D SLAVE_ID=1
    -D SLAVE_TYPE=7
    -D DEVICE_NAME="PjonSlave1"
```

### 3. Upload and Test
```bash
# Flash specific slave
pio run -e slave_1 --target upload

# Monitor output
pio device monitor -e slave_1
```

## 📋 **Command Reference Table**

| Command | Hex  | Powerplant Types | Description |
|---------|------|------------------|-------------|
| CMD_ON | 0x01 | Wind, Nuclear, Hydro, Coal | Turn ON |
| CMD_OFF | 0x02 | All except Battery | Turn OFF |
| BAT_IDLE | 0x03 | Battery, Photovoltaic | Idle/Default state |
| BAT_CHARGING | 0x04 | Battery | Charging state |
| BAT_DISCHARGE | 0x05 | Battery | Discharging state |
| GAS_LEVEL_1 | 0x06 | Gas | Level 1 - Green |
| GAS_LEVEL_2 | 0x07 | Gas | Level 2 - Yellow-Green |
| GAS_LEVEL_3 | 0x08 | Gas | Level 3 - Blue |
| GAS_LEVEL_4 | 0x09 | Gas | Level 4 - Orange |
| GAS_LEVEL_5 | 0x0A | Gas | Level 5 - Red |
| HYDRO_STORAGE_LEVEL_1 | 0x0B | Hydro Storage | 100% Full - Green |
| HYDRO_STORAGE_LEVEL_2 | 0x0C | Hydro Storage | 75% - Light Green |
| HYDRO_STORAGE_LEVEL_3 | 0x0D | Hydro Storage | 50% - Orange |
| HYDRO_STORAGE_LEVEL_4 | 0x0E | Hydro Storage | 25% - Light Red |
| HYDRO_STORAGE_LEVEL_5 | 0x0F | Hydro Storage | 0% Empty - Red |

## 🔧 **Hardware Requirements**

- **MCU**: ESP8266 (D1 Mini recommended)
- **Communication**: StarWire protocol (2-wire: DATA + CLK)
- **Peripherals**:
  - RGB LED (WS2812B) for Battery, Gas, Photovoltaic, Hydro Storage
  - Atomizer for Nuclear, Coal
  - Single wire motor controller for Wind (D2), Hydro (D5)
  - Solar panel sensor (A0) for Photovoltaic
- **Power**: 3.3V stable supply

## 🌐 **Remote Access**

### WebSerial Interface
- **URL**: `http://StarWireSlave.local/webserial`
- **Features**: Real-time monitoring, command logs, debug output
- **Data**: Command responses, sensor readings, state changes

### OTA Updates
- **Initial**: USB upload required
- **Subsequent**: Wireless via `pio run -e slave_X_ota --target upload`
- **Hostname**: `StarWireSlave.local` (or device-specific name)

## 📊 **Expected Output Examples**

### Battery Module
```
StarWire Slave booting...
Slave ready: ID=9, Type=8
[BAT] cmd=0x4 from master 1
Battery: Charging -> RED
```

### Gas Module
```
StarWire Slave booting...
Slave ready: ID=6, Type=4
Gas powerplant LED initialized on D2
[GAS] cmd=0x8 from master 1
Gas: Level 3 -> BLUE (Medium production)
```

### Wind Module
```
StarWire Slave booting...
Slave ready: ID=4, Type=2
Wind motor initialized on D2 with speedup enabled
[WIND] cmd=0x1 from master 1
[WIND] Motor ON -> 50 power level
```

### Photovoltaic Module
```
StarWire Slave booting...
Slave ready: ID=3, Type=1
Solar panel initialized on A0, LED on D2
[SOLAR] A0=512, V=1.65V, Bright=128, Mode=0, RGB=(127,255,0)
```

### Hydro Module
```
StarWire Slave booting...
Slave ready: ID=7, Type=5
Hydro motor initialized on D5
[HYDRO] cmd=0x1 from master 1
[HYDRO] Motor ON -> 100%
```

### Hydro Storage Module
```
StarWire Slave booting...
Slave ready: ID=8, Type=6
Hydro Storage LED initialized on D2
[HYDRO_STORAGE] cmd=0xB from master 1
Hydro Storage: Level 1 -> GREEN (100% - Full/Discharging)
```

## 🛠️ **Configuration Generator**

Use the included Python script to generate configurations:

```bash
# Generate all powerplant types
python config_generator.py update 1 2 --type 7  # 2 Coal plants
python config_generator.py update 3 3 --type 1  # 1 Photovoltaic  
python config_generator.py update 4 4 --type 2  # 1 Wind
python config_generator.py update 5 5 --type 3  # 1 Nuclear
python config_generator.py update 6 6 --type 4  # 1 Gas
python config_generator.py update 7 7 --type 5  # 1 Hydro
python config_generator.py update 8 8 --type 6  # 1 Hydro Storage
python config_generator.py update 9 9 --type 8  # 1 Battery

# List existing configurations
python config_generator.py list

# Generate flash script
python config_generator.py flash-script 1 2 3 4 5 6 7 8 9
```

## 🔧 **Troubleshooting**

### No Communication
1. Check D1 (DATA) and D7 (CLK) connections
2. Verify pull-up resistors on communication lines
3. Ensure unique slave IDs across all devices
4. Check power supply stability

### LED/Atomizer Issues
1. Verify D2 pin connections
2. Check power supply capacity for RGB LEDs
3. For atomizers, ensure proper water level
4. Check peripheral library compatibility

### Photovoltaic Sensor Issues
1. Verify A0 pin connection to solar panel
2. Check voltage divider if input > 3.3V
3. Monitor serial output for sensor readings
4. Ensure proper lighting conditions for testing

## 🏭 **Complete Powerplant System Overview**

The system now supports 8 different powerplant types, each with unique hardware and control characteristics:

1. **Photovoltaic (Solar)** - Light-responsive RGB LED with 3 modes and brightness control
2. **Wind** - Motor controller with speedup functionality (50 power level, 2.5x speedup)
3. **Nuclear** - Simple atomizer ON/OFF control for steam effects
4. **Gas** - 5-level RGB LED production indicator (Green→Yellow-Green→Blue→Orange→Red)
5. **Hydro** - Motor controller for turbine simulation (100% power level)
6. **Hydro Storage** - 5-level RGB LED storage indicator (100%→75%→50%→25%→0%)
7. **Coal** - Simple atomizer ON/OFF control for steam effects
8. **Battery** - 3-level RGB LED status indicator (Idle/Charging/Discharging)

Each powerplant type provides realistic visual and mechanical feedback appropriate for energy simulation and educational purposes.

## 📚 **Development**

### Adding Custom Powerplant Types
1. Define new `TYPE_CUSTOM` constant
2. Add to supported types check
3. Create handler function
4. Register commands in setup()
5. Initialize peripherals

### Extending Gas Powerplant Levels
```cpp
// Add more levels
static const uint8_t GAS_LEVEL_6 = 0x0B;

// Update handler
case GAS_LEVEL_6:
    gasLed->setColor(255, 0, 255); // Purple
    gasLed->setBrightness(255);
    break;
```

---

**Ready for deployment!** This comprehensive powerplant module system provides realistic control and feedback for energy simulation systems.
