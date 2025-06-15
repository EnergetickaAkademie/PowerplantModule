# OTA (Over-The-Air) Setup Guide

## Overview
Your project now supports OTA updates, allowing you to upload new firmware wirelessly without connecting a USB cable.

## Configuration Details
- **Hostname**: `OneWireSlave`
- **Password**: `admin` 
- **Port**: Uses mDNS resolution (`OneWireSlave.local`)

## How to Use OTA

### 1. Initial Upload (USB Required)
First, upload the firmware via USB using the regular environment:
```bash
pio run -e d1_mini --target upload
```

### 2. Monitor Serial Output
After upload, open the serial monitor to see the device connect to WiFi:
```bash
pio device monitor
```

You should see output like:
```
OneWire-Hub DS18B20 Temperature-Sensor with OTA
Connecting to WiFi.....
Connected! IP address: 192.168.1.xxx
OTA Ready
```

### 3. Subsequent Updates (Wireless)
Once the device is running and connected to WiFi, you can upload updates wirelessly:
```bash
pio run -e d1_mini_ota --target upload
```

## Network Requirements
- Device and computer must be on the same network
- mDNS must be working (most home networks support this)
- If mDNS fails, you can modify `platformio.ini` to use the IP address directly

## Security Notes
- **Change the OTA password** in production environments
- The password is currently set to `admin` in the code
- WiFi credentials are stored in `include/secrets.h`

## Troubleshooting

### Can't Find Device
If OTA upload fails to find the device:
1. Check that the device is connected to WiFi (check serial monitor)
2. Verify both devices are on the same network
3. Try using IP address instead of hostname in `platformio.ini`:
   ```ini
   upload_port = 192.168.1.xxx  ; Replace with actual IP
   ```

### Upload Fails
- Ensure the device is not performing critical operations during upload
- The device should be in a stable state (not constantly polling OneWire)
- Try uploading when the device is idle

## Environment Configurations

### `d1_mini` Environment
- Regular USB upload
- Use for initial firmware installation
- Use when OTA is not available

### `d1_mini_ota` Environment  
- OTA upload via WiFi
- Use for subsequent updates
- Requires device to be running and connected to WiFi

## Code Features
- Automatic WiFi connection on startup
- OTA progress monitoring via serial output
- Error handling for OTA failures
- Compatible with existing OneWire functionality
