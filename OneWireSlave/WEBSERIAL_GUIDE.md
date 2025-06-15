# WebSerial Remote Debug Guide

## Overview
Your project now includes WebSerial, a web-based serial monitor that allows you to view debug output and send commands through your browser instead of using a USB connection.

## What is WebSerial?
WebSerial provides a web interface accessible through your ESP8266's IP address that acts like a serial monitor. It's perfect for debugging devices that are deployed remotely or when you don't want to use USB cables.

## Configuration Details
- **Web Interface**: Available at `http://[IP_ADDRESS]/webserial`
- **mDNS Access**: `http://OneWireSlave.local/webserial`
- **Port**: 80 (HTTP)
- **Bidirectional**: Can both receive debug output and send commands to device

## How to Access WebSerial

### 1. Initial Setup (USB Required)
Upload the firmware via USB for the first time:
```bash
pio run -e d1_mini --target upload
```

### 2. Monitor Serial Output
Connect via USB serial to see the device's IP address:
```bash
pio device monitor
```

Look for output like:
```
OneWire-Hub DS18B20 Temperature-Sensor with OTA
Booting
Connecting to WiFi.....
Connected! IP address: 192.168.1.xxx
WebSerial initialized!
Open browser and go to: http://192.168.1.xxx/webserial
Or use: http://OneWireSlave.local/webserial
```

### 3. Access WebSerial Interface
Open your web browser and navigate to:
- `http://192.168.1.xxx/webserial` (replace with actual IP)
- OR `http://OneWireSlave.local/webserial` (using mDNS)

## Features

### Debug Output
The device sends debug information to WebSerial including:
- Temperature updates every second
- OneWire Hub error messages
- Connection status
- OTA update progress

### Interactive Commands
You can send commands from the web interface to the device. Currently implemented:
- Any text you type will be echoed back to confirm bidirectional communication
- Commands are also printed to the USB serial for debugging

### Real-time Monitoring
- See live temperature readings as they update
- Monitor OneWire communication errors
- Track device status without physical access

## Current Debug Output
The device currently sends these messages to WebSerial:

1. **Temperature Updates**: Every second when the LED blinks
   ```
   Temperature set to: 20.1
   Temperature set to: 20.2
   ...
   ```

2. **Error Messages**: When OneWire Hub encounters issues
   ```
   OneWire Hub Error detected!
   ```

3. **Interactive Responses**: When you send commands
   ```
   Received Data: your_command_here
   ```

## Advantages Over USB Serial
- **Remote Access**: Debug devices anywhere on your network
- **Multiple Connections**: Multiple people can view the same debug output
- **Web-based**: No special software needed, just a web browser
- **Always Available**: Works 24/7 as long as the device is powered and connected to WiFi
- **Mobile Friendly**: Access from phones and tablets

## Network Requirements
- Device and computer must be on the same network
- Firewall should allow HTTP traffic (port 80)
- mDNS should be working for `.local` access (most home networks support this)

## Troubleshooting

### Can't Access Web Interface
1. Verify the device is connected to WiFi (check serial output)
2. Try using IP address instead of `.local` hostname
3. Check that both devices are on the same network
4. Ensure firewall isn't blocking port 80

### No Debug Output
1. Check that the device is running and operational
2. Refresh the browser page
3. Verify WebSerial is properly initialized (check serial output)

### Commands Not Working
1. Make sure you're typing in the input field and pressing Enter
2. Check the serial monitor to see if commands are received
3. Commands are echoed back, so you should see your input repeated

## Future Enhancements
You can extend the WebSerial functionality by:
- Adding specific command handlers for device control
- Implementing different debug levels (INFO, WARNING, ERROR)
- Adding device configuration through web commands
- Creating custom web interface elements

## Code Integration
WebSerial is integrated into your code in these locations:
- `lib/ota/ota.h` - Header declarations
- `lib/ota/ota.cpp` - Implementation and setup
- `src/main.cpp` - Usage in main application

The integration is non-blocking and doesn't interfere with your OneWire operations.
