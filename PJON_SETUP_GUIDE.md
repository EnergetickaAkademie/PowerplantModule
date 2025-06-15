# PJON Temperature Sensor Network Setup Guide

## Overview

Your OneWire projects have been converted to use PJON (Padded Jittering Operative Network), a more robust and reliable communication protocol. PJON provides better error handling, acknowledgments, collision detection, and retry logic compared to OneWire.

## 🎯 **PJON Advantages Over OneWire**

- **Robust Error Handling**: Built-in acknowledgments and retry logic
- **Better Timing Tolerance**: Less sensitive to timing variations
- **Multi-Master Support**: Multiple masters can coexist on the same bus
- **Collision Detection**: Automatic collision detection and retry
- **Flexible Addressing**: Easy device ID management
- **Protocol Abstraction**: Higher-level message-based communication

## 🚀 **Quick Start**

### Hardware Setup
1. Connect all ESP8266 devices to the same communication bus (pin D1)
2. Add a 4.7kΩ pull-up resistor between the bus line and 3.3V
3. Connect common ground between all devices
4. Power all devices

### Device Configuration

#### Master Device (OneWireHost)
- **Device ID**: 1 (fixed)
- **Role**: Polls slaves for temperature data and sends heartbeats
- **Hostname**: `PjonMaster`

#### Slave Devices (OneWireSlave)
- **Device IDs**: 10, 11, 12, ... (configurable)
- **Role**: Responds to temperature requests and heartbeats
- **Hostname**: `PjonSlave`

## 📋 **Configuration Steps**

### 1. Master Configuration
No changes needed in the code. The master will automatically:
- Poll slaves with IDs 10, 11, 12 for temperature data
- Send heartbeats every 10 seconds
- Display temperature summary every 15 seconds

### 2. Slave Configuration
Edit `src/main.cpp` in each slave device:

```cpp
#define SLAVE_ID 10  // Change this for each slave (10, 11, 12, etc.)
```

### 3. Adding More Slaves
To add more slave devices, update the master's `slave_ids` array:

```cpp
uint8_t slave_ids[] = {10, 11, 12, 13, 14}; // Add new IDs here
```

## 🔧 **Hardware Wiring**

```
Master (ID:1)      Slave (ID:10)     Slave (ID:11)     Slave (ID:12)
     |                   |                 |                 |
     +-------[4.7kΩ]-----+                 |                 |
     |                   |                 |                 |
   GPIO2 (D1)          GPIO2 (D1)        GPIO2 (D1)       GPIO2 (D1)
     |                   |                 |                 |
     +-------------------+-----------------+-----------------+  (PJON Bus)
     |                   |                 |                 |
    GND                 GND               GND               GND
     |                   |                 |                 |
     +-------------------+-----------------+-----------------+  (Common Ground)
```

**Important**: Only ONE 4.7kΩ pull-up resistor is needed for the entire bus!

## 📊 **Protocol Messages**

### Message Types
- `TEMP_REQUEST (0x01)`: Master requests temperature from slave
- `TEMP_RESPONSE (0x02)`: Slave responds with temperature data
- `HEARTBEAT (0x03)`: Master sends heartbeat to keep slaves alive
- `STATUS_REQUEST (0x04)`: Master requests device status
- `STATUS_RESPONSE (0x05)`: Slave responds with status information

### Temperature Response Format
```
Byte 0: TEMP_RESPONSE (0x02)
Bytes 1-4: Temperature as float (4 bytes)
Byte 5: Slave ID
```

## 🖥️ **Expected Output**

### Master Device Serial Output
```
PJON Temperature Master
Ready
IP address: 192.168.1.100
PJON Master initialized with ID: 1
Communication pin: 5

Temperature request sent to slave 10
Received message from slave ID 10, type: 2
Temperature from slave 10: 23.45°C

Heartbeat sent to slave 10
Heartbeat sent to slave 11

=== Temperature Summary ===
Slave 10: 23.45°C (age: 2s)
Slave 11: 21.30°C (age: 5s)
==========================
```

### Slave Device Serial Output
```
PJON Temperature Sensor Slave
Ready
IP address: 192.168.1.101
PJON Slave initialized with ID: 10
Listening on pin: 5

Received message from ID 1, type: 1
Temperature request received
Temperature response sent successfully

Received message from ID 1, type: 3
Heartbeat received
```

## 🌐 **WebSerial Monitoring**

Both master and slave devices support WebSerial for remote monitoring:

- **Master**: `http://PjonMaster.local/webserial`
- **Slave**: `http://PjonSlave.local/webserial`

WebSerial displays:
- Real-time temperature readings
- Communication status
- Error messages
- Device status information

## 🔧 **Development Workflow**

### Initial Upload (USB)
```bash
cd OneWireHost  # or OneWireSlave
pio run -e d1_mini --target upload
```

### Wireless Updates (OTA)
```bash
pio run -e d1_mini_ota --target upload
```

### Monitor Output
```bash
pio device monitor
```

## 🛠️ **Troubleshooting**

### No Communication
1. **Check Wiring**: Verify all devices are connected to the same bus
2. **Pull-up Resistor**: Ensure 4.7kΩ resistor is connected
3. **Device IDs**: Verify unique slave IDs and correct master configuration
4. **Power Supply**: Check all devices have stable power

### Slaves Not Responding
1. **Check Slave IDs**: Ensure each slave has a unique ID
2. **Monitor Serial**: Check if slaves receive requests
3. **WebSerial**: Use web interface to debug remotely
4. **Bus Load**: Too many devices can cause collisions

### Communication Errors
1. **Signal Quality**: Check for electrical interference
2. **Cable Length**: Keep bus wires short (< 10m for reliable operation)
3. **Timing**: PJON is more tolerant but very long cables can still cause issues

## 📈 **Performance Characteristics**

### PJON vs OneWire Comparison
| Feature | OneWire | PJON |
|---------|---------|------|
| Error Handling | Basic CRC | ACK/NAK + Retry |
| Timing Tolerance | Very strict | Flexible |
| Multi-Master | No | Yes |
| Collision Detection | No | Yes |
| Max Devices | ~100 | Limited by addressing |
| Protocol Complexity | Low-level | High-level |

### Network Capacity
- **Recommended**: Up to 10 devices per bus
- **Maximum**: Depends on message frequency and bus length
- **Performance**: ~1-10 messages per second typical

## 🔒 **Security Considerations**

- Device IDs are not encrypted
- Messages are sent in plain text
- Consider physical bus security
- WiFi credentials should be secured in `secrets.h`

## 🚀 **Advanced Features**

### Custom Message Types
Add new message types by extending the enum:

```cpp
enum MessageType {
    TEMP_REQUEST = 0x01,
    TEMP_RESPONSE = 0x02,
    HEARTBEAT = 0x03,
    STATUS_REQUEST = 0x04,
    STATUS_RESPONSE = 0x05,
    CUSTOM_COMMAND = 0x10  // Add custom commands
};
```

### Error Recovery
PJON automatically handles:
- Message acknowledgments
- Automatic retries on failure
- Collision detection and backoff
- Bus arbitration

### Network Monitoring
The master provides comprehensive monitoring:
- Temperature history per slave
- Communication statistics
- Device availability tracking
- WebSerial real-time updates

## 📚 **Further Reading**

- [PJON GitHub Repository](https://github.com/gioblu/PJON)
- [PJON Documentation](https://github.com/gioblu/PJON/wiki)
- [SoftwareBitBang Strategy](https://github.com/gioblu/PJON/wiki/SoftwareBitBang)

---

**Ready to use!** Your temperature sensor network is now more robust and reliable with PJON protocol.
