# PJON Slave Management Guide

This guide explains how to manage multiple PJON slaves with automatic configuration and easy USB/OTA deployment.

## Overview

The system supports:
- **USB Flashing**: Initial deployment via USB cable
- **OTA Updates**: Over-the-air updates after initial deployment
- **Automatic Configuration**: Each slave gets unique ID, type, and device name
- **Easy Management**: Scripts for batch operations

## Quick Start

### 1. List Available Slaves
```bash
./manage_slaves.sh list
```

### 2. Flash a Single Slave (First Time)
```bash
# Connect ESP8266 via USB
./manage_slaves.sh flash 10
```

### 3. Update via OTA (After First Flash)
```bash
./manage_slaves.sh ota 10
```

### 4. Monitor Slave Output
```bash
./manage_slaves.sh monitor 10
```

### 5. Check if Slave is Online
```bash
./manage_slaves.sh ping 10
```

## Batch Operations

### Flash All Slaves via USB
```bash
./manage_slaves.sh flash-all
```
*You'll be prompted to connect each ESP8266 one by one*

### Update All Online Slaves via OTA
```bash
./manage_slaves.sh ota-all
```

## Adding New Slaves

### Method 1: Using Python Generator
```bash
# Add slaves 15-20 with type 2
./config_generator.py update 15 20 --type 2

# Generate flash script for specific slaves
./config_generator.py flash-script 15 16 17
```

### Method 2: Manual platformio.ini Edit
Add to `platformio.ini`:
```ini
[env:slave_15]
extends = env
build_flags = 
    -D SLAVE_ID=15
    -D SLAVE_TYPE=2
    -D DEVICE_NAME=\"PjonSlave15\"

[env:slave_15_ota]
extends = env:slave_15
upload_protocol = espota
upload_port = PjonSlave15.local
upload_flags = --auth=admin
```

## Configuration Structure

### platformio.ini Structure
```ini
[env]                    # Base configuration
[env:d1_mini]           # Default development environment
[env:slave_X]           # USB flash environment for slave X
[env:slave_X_ota]       # OTA update environment for slave X
```

### Build Flags
- `SLAVE_ID`: Unique identifier (10, 11, 12, ...)
- `SLAVE_TYPE`: Type classification (1=temp sensor, 2=relay, 3=input, ...)
- `DEVICE_NAME`: Network hostname (PjonSlaveX)

### Network Addresses
Each slave will be available at: `PjonSlaveX.local`
- Slave 10: `PjonSlave10.local`
- Slave 11: `PjonSlave11.local`
- etc.

## Workflow

### Initial Deployment
1. **Configure**: Add slave to `platformio.ini`
2. **Flash USB**: `./manage_slaves.sh flash X`
3. **Verify**: `./manage_slaves.sh ping X`

### Updates
1. **Code Changes**: Modify source code
2. **OTA Update**: `./manage_slaves.sh ota X`
3. **Batch Update**: `./manage_slaves.sh ota-all`

### Troubleshooting
1. **Check Status**: `./manage_slaves.sh list`
2. **Monitor Output**: `./manage_slaves.sh monitor X`
3. **Re-flash**: `./manage_slaves.sh flash X` (if OTA fails)

## Command Reference

### manage_slaves.sh Commands
```bash
./manage_slaves.sh flash <id>      # Flash via USB
./manage_slaves.sh ota <id>        # Update via OTA
./manage_slaves.sh list            # List all slaves
./manage_slaves.sh monitor <id>    # Monitor serial output
./manage_slaves.sh ping <id>       # Check network connectivity
./manage_slaves.sh flash-all       # Flash all via USB
./manage_slaves.sh ota-all         # Update all via OTA
```

### config_generator.py Commands
```bash
./config_generator.py generate 10 15 --type 1    # Generate configs
./config_generator.py update 10 15 --type 1      # Add to platformio.ini
./config_generator.py list                       # List existing slaves
./config_generator.py flash-script 10 11 12      # Generate flash script
```

### Direct PlatformIO Commands
```bash
# USB flash
pio run -e slave_10 -t upload

# OTA update
pio run -e slave_10_ota -t upload

# Monitor
pio device monitor -e slave_10

# Build only
pio run -e slave_10
```

## Example Scenarios

### Scenario 1: Deploy 5 New Temperature Sensors
```bash
# 1. Add configurations
./config_generator.py update 20 24 --type 1

# 2. Flash all via USB
./manage_slaves.sh flash-all

# 3. Later, update all
./manage_slaves.sh ota-all
```

### Scenario 2: Add Single Relay Controller
```bash
# 1. Add configuration
./config_generator.py update 30 30 --type 2

# 2. Flash via USB
./manage_slaves.sh flash 30

# 3. Test connectivity
./manage_slaves.sh ping 30

# 4. Update via OTA
./manage_slaves.sh ota 30
```

### Scenario 3: Firmware Update Deployment
```bash
# 1. Check which slaves are online
./manage_slaves.sh list

# 2. Update all online slaves
./manage_slaves.sh ota-all

# 3. Flash any offline slaves individually
./manage_slaves.sh flash 15  # if slave 15 was offline
```

## Slave Types

Define different slave types using `SLAVE_TYPE`:

| Type | Description | Use Case |
|------|-------------|----------|
| 1 | Temperature Sensor | Environmental monitoring |
| 2 | Relay Controller | Light/appliance control |
| 3 | Digital Input | Button/switch monitoring |
| 4 | Analog Sensor | Variable measurements |
| 5 | PWM Output | Dimming/motor control |

## Network Configuration

### WiFi Settings
Configure in `include/secrets.h`:
```cpp
#define SECRET_SSID "YourWiFiNetwork"
#define SECRET_PASSWORD "YourWiFiPassword"
```

### mDNS Names
Each slave automatically registers as:
- `PjonSlaveX.local` (main hostname)
- WebSerial available at `http://PjonSlaveX.local/webserial`

## Troubleshooting

### Common Issues

1. **OTA Upload Fails**
   - Check WiFi connectivity: `ping PjonSlaveX.local`
   - Re-flash via USB if needed
   - Verify firewall settings

2. **Slave Not Responding**
   - Check power supply
   - Monitor serial output
   - Verify network connectivity

3. **Compilation Errors**
   - Check `SLAVE_ID` is unique
   - Verify `platformio.ini` syntax
   - Update library dependencies

### Debug Steps
```bash
# 1. Check slave status
./manage_slaves.sh list

# 2. Monitor specific slave
./manage_slaves.sh monitor 10

# 3. Test network connectivity
./manage_slaves.sh ping 10

# 4. Re-flash if needed
./manage_slaves.sh flash 10
```

## Best Practices

1. **Use Sequential IDs**: Start from 10, increment by 1
2. **Document Types**: Keep a record of which type means what
3. **Test Before Deployment**: Always test one slave before batch operations
4. **Keep Backups**: Save working firmware versions
5. **Monitor Status**: Regularly check slave connectivity
6. **Plan Updates**: Schedule OTA updates during maintenance windows

This system makes managing dozens of PJON slaves simple and efficient!
