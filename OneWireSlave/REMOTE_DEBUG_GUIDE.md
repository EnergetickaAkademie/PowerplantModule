# Remote Debug Guide

## Overview
Your OneWire Slave project now supports remote debugging via telnet, allowing you to see debug output over WiFi instead of needing a USB connection.

## Features Added
- **RemoteDebug Library**: Telnet-based debugging over WiFi
- **mDNS Support**: Use `OneWireSlave.local` instead of IP addresses
- **Multiple Debug Levels**: Info, Warning, Error, Debug, Verbose
- **Color Output**: Different colors for different log levels
- **Timestamps**: Each log entry includes timestamp
- **Remote Commands**: Send commands to the device remotely

## How to Connect

### Method 1: Using Hostname (Recommended)
```bash
telnet OneWireSlave.local 23
```

### Method 2: Using IP Address
```bash
telnet <DEVICE_IP> 23
```

You can find the device IP in the serial monitor output when it boots.

## Debug Output Examples
The device will show various debug messages:
- **Info (Green)**: `debugI("Temperature updated: 21.5°C")`
- **Warning (Yellow)**: `debugW("Sensor reading unstable")`
- **Error (Red)**: `debugE("OneWire Hub Error detected!")`
- **Debug (Blue)**: `debugD("Detailed debug info")`
- **Verbose (White)**: `debugV("Verbose output")`

## Remote Commands
Once connected via telnet, you can use these commands:

### Basic Commands
- `h` or `help` - Show help
- `r` - Reset ESP8266
- `s` - Show status
- `q` - Quit telnet session

### Debug Level Commands
- `v` - Set debug level to Verbose
- `d` - Set debug level to Debug  
- `i` - Set debug level to Info
- `w` - Set debug level to Warning
- `e` - Set debug level to Error

### Profiler Commands
- `p` - Show profiler (if enabled)
- `c` - Show/hide colors
- `t` - Show/hide time

## Connection Examples

### Linux/macOS
```bash
# Connect using hostname
telnet OneWireSlave.local 23

# Or using IP
telnet 192.168.1.100 23
```

### Windows
```cmd
# Connect using hostname
telnet OneWireSlave.local 23

# Or using IP  
telnet 192.168.1.100 23
```

### PuTTY (Windows)
1. Open PuTTY
2. Select "Telnet" connection type
3. Enter hostname: `OneWireSlave.local` or IP address
4. Port: `23`
5. Click "Open"

## Debug Output You'll See
```
* INFO: OneWire Hub initialized
* INFO: DS18B20 sensor attached with initial temperature: 21°C
* INFO: Temperature updated: 20.1°C
* INFO: Temperature updated: 20.2°C
* ERROR: OneWire Hub Error detected!
```

## Integration in Your Code

### Available Debug Macros
```cpp
debugV("Verbose message");          // Verbose (most detailed)
debugD("Debug message");            // Debug
debugI("Info message");             // Info (default level)
debugW("Warning message");          // Warning
debugE("Error message");            // Error (least detailed)

// With format strings
debugI("Temperature: %.1f°C", temp);
debugW("Sensor %d reading: %d", id, value);
```

### Conditional Debug
```cpp
if (Debug.isActive(Debug.INFO)) {
    debugI("This only prints if Info level is active");
}
```

## Troubleshooting

### Cannot Connect via Telnet
1. **Check WiFi**: Ensure device is connected to WiFi
2. **Check IP**: Verify the device IP from serial monitor
3. **Check Network**: Ensure your computer and device are on same network
4. **Try IP instead of hostname**: If mDNS isn't working
5. **Check Firewall**: Ensure port 23 isn't blocked

### mDNS Not Working
- On Windows: Install Bonjour Print Services
- On Linux: Install `avahi-daemon`
- Fallback: Use IP address directly

### No Debug Output
1. Check debug level is set appropriately
2. Verify `Debug.handle()` is called in main loop
3. Check if debugging is active: `Debug.isActive()`

## Performance Notes
- Remote debugging has minimal impact on performance
- Debug output is buffered and sent efficiently
- You can disable debug levels in production to save resources
- The telnet server runs on port 23 by default

## Security Considerations
- Telnet is unencrypted - use only on trusted networks
- No authentication required - anyone on network can connect
- Debug output may contain sensitive information
- Consider disabling in production environments

## Code Integration Summary
The integration includes:
1. **Library**: RemoteDebug added to platformio.ini
2. **Initialization**: `setupRemoteDebug()` called in setup()
3. **Handling**: `Debug.handle()` called in main loop
4. **Usage**: Debug macros used throughout code
5. **mDNS**: Hostname resolution for easy connection
