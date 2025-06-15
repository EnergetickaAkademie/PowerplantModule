# PJON Slave Management - Python Edition

A powerful Python-based tool for managing multiple PJON slaves with automatic configuration parsing and easy USB/OTA deployment.

## Features

✅ **Reliable Parsing**: Python-based configuration parsing (no more bash regex issues!)  
✅ **USB + OTA Support**: Initial USB flashing, then OTA updates  
✅ **Batch Operations**: Flash or update multiple slaves at once  
✅ **Network Detection**: Automatic online/offline status checking  
✅ **Build Testing**: Build firmware without uploading  
✅ **Colored Output**: Clear status messages with color coding  
✅ **Error Handling**: Robust error handling and helpful messages  

## Quick Start

### 1. List Available Slaves
```bash
./manage_slaves.py list
```

### 2. Flash a Single Slave (First Time - USB)
```bash
# Connect ESP8266 via USB, put in programming mode
./manage_slaves.py flash 10
```

### 3. Update via OTA (After Initial Flash)
```bash
./manage_slaves.py ota 10
```

### 4. Monitor Slave Serial Output
```bash
./manage_slaves.py monitor 10
```

### 5. Check Network Connectivity
```bash
./manage_slaves.py ping 10
```

## All Commands

### Individual Slave Operations
```bash
./manage_slaves.py flash <id>      # Flash via USB (first time)
./manage_slaves.py ota <id>        # Update via OTA
./manage_slaves.py build <id>      # Build firmware (test compilation)
./manage_slaves.py monitor <id>    # Monitor serial output
./manage_slaves.py ping <id>       # Check network connectivity
```

### Batch Operations
```bash
./manage_slaves.py flash-all       # Flash all slaves via USB (interactive)
./manage_slaves.py ota-all         # Update all online slaves via OTA
./manage_slaves.py build-all       # Build all slave configurations
./manage_slaves.py list            # List all configured slaves
```

## Example Output

### List Command
```
[INFO] Configured slaves:

  Slave 10:
    Type: 1
    Device Name: PjonSlave10
    OTA Address: PjonSlave10.local
    Status: Online

  Slave 11:
    Type: 1
    Device Name: PjonSlave11
    OTA Address: PjonSlave11.local
    Status: Offline

  Slave 12:
    Type: 2
    Device Name: PjonSlave12
    OTA Address: PjonSlave12.local
    Status: Online
```

### Flash Command
```
[INFO] Flashing slave 10 via USB...
[WARNING] Make sure the ESP8266 is connected via USB and in programming mode
[SUCCESS] Slave 10 flashed successfully!
[INFO] The slave will now be available as PjonSlave10.local for OTA updates
```

### OTA Update
```
[INFO] Updating slave 10 via OTA...
[INFO] Trying to reach PjonSlave10.local...
[SUCCESS] Slave 10 is reachable
[SUCCESS] Slave 10 updated successfully via OTA!
```

## Configuration Management

### Current platformio.ini Structure
The script automatically parses your `platformio.ini` file:

```ini
[env:slave_10]
extends = env
build_flags = 
    -D SLAVE_ID=10
    -D SLAVE_TYPE=1
    -D DEVICE_NAME=\"PjonSlave10\"

[env:slave_10_ota]
extends = env:slave_10
upload_protocol = espota
upload_port = PjonSlave10.local
upload_flags = --auth=admin
```

### Adding New Slaves
Use the `config_generator.py` script:

```bash
# Add slaves 15-20 with type 2
./config_generator.py update 15 20 --type 2

# Generate flash script for specific slaves  
./config_generator.py flash-script 15 16 17
```

## Workflow Examples

### Scenario 1: Deploy New Slave
```bash
# 1. Add to platformio.ini (manual or via config_generator.py)
./config_generator.py update 25 25 --type 1

# 2. Build and test
./manage_slaves.py build 25

# 3. Flash via USB (first time)
./manage_slaves.py flash 25

# 4. Test connectivity
./manage_slaves.py ping 25

# 5. Future updates via OTA
./manage_slaves.py ota 25
```

### Scenario 2: Batch USB Deployment
```bash
# Flash multiple slaves (interactive)
./manage_slaves.py flash-all
# Script will prompt you to connect each slave one by one
```

### Scenario 3: Firmware Update Rollout
```bash
# 1. Check which slaves are online
./manage_slaves.py list

# 2. Build all configurations first (test)
./manage_slaves.py build-all

# 3. Update all online slaves
./manage_slaves.py ota-all

# 4. Flash any offline slaves individually when they come online
./manage_slaves.py flash 15  # if slave 15 was offline
```

### Scenario 4: Debugging Issues
```bash
# Check slave status
./manage_slaves.py list

# Test network connectivity
./manage_slaves.py ping 10

# Monitor serial output
./manage_slaves.py monitor 10

# Re-flash if OTA fails
./manage_slaves.py flash 10
```

## Advanced Features

### Network Detection
The script automatically detects if slaves are online using:
- DNS resolution (`PjonSlaveX.local`)
- ICMP ping with timeout
- Proper error handling

### Build Verification
Test compilation without uploading:
```bash
./manage_slaves.py build 10      # Single slave
./manage_slaves.py build-all     # All slaves
```

### Robust Error Handling
- Validates slave configurations exist
- Checks PlatformIO availability
- Handles network timeouts
- Provides helpful error messages

## Comparison: Python vs Bash

| Feature | Bash Script | Python Script |
|---------|-------------|---------------|
| Configuration Parsing | ❌ Unreliable regex | ✅ Robust regex + string handling |
| Error Handling | ⚠️ Basic | ✅ Comprehensive |
| Network Detection | ⚠️ Simple ping | ✅ DNS + ping with timeout |
| Code Maintainability | ❌ Complex | ✅ Clean, readable |
| Cross-platform | ⚠️ Linux/Mac only | ✅ Windows/Linux/Mac |
| Colored Output | ✅ Yes | ✅ Yes |
| Batch Operations | ✅ Yes | ✅ Yes |

## File Structure

```
OneWireSlave/
├── manage_slaves.py      # Main Python script
├── manage_slaves         # Bash wrapper (optional)
├── manage_slaves.sh      # Old bash script (deprecated)
├── config_generator.py   # Configuration generator
├── platformio.ini        # Slave configurations
└── src/main.cpp          # Slave firmware
```

## Requirements

- Python 3.6+
- PlatformIO Core
- Network access for OTA updates

## Troubleshooting

### "platformio.ini not found"
- Run the script from the OneWireSlave directory
- Make sure you're in the correct workspace

### "Slave X configuration not found"
- Check if the slave is defined in platformio.ini
- Use `./config_generator.py list` to see existing slaves

### "Cannot reach PjonSlaveX.local"
- Verify slave is powered on and connected to WiFi
- Check network connectivity
- Try USB flash if OTA fails

### Build/Upload Failures
- Check PlatformIO installation: `pio --version`
- Verify USB cable connection (for flash)
- Check WiFi connectivity (for OTA)

## Migration from Bash Script

If you were using the old bash script:

1. **All commands are the same**: Just replace `./manage_slaves.sh` with `./manage_slaves.py`
2. **Better reliability**: No more parsing errors
3. **More features**: Added `build` commands and better error handling
4. **Backward compatible**: All your existing platformio.ini configurations work

The Python script is a drop-in replacement with better reliability and additional features!
