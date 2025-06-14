# ESP8266 OneWire Master-Slave Setup Guide

## Quick Setup Instructions

### Device Configuration

1. **Master Device**: Set these values in `src/main.cpp`:
```cpp
#define DEVICE_MODE_MASTER true
#define SLAVE_ID 1               // Not used for master
```

2. **Slave Device 1**: Set these values in `src/main.cpp`:
```cpp
#define DEVICE_MODE_MASTER false
#define SLAVE_ID 1               // This slave will blink 1 time
```

3. **Slave Device 2**: Set these values in `src/main.cpp`:
```cpp
#define DEVICE_MODE_MASTER false
#define SLAVE_ID 2               // This slave will blink 2 times
```

4. **Slave Device 3**: Set these values in `src/main.cpp`:
```cpp
#define DEVICE_MODE_MASTER false
#define SLAVE_ID 3               // This slave will blink 3 times
```

### Hardware Wiring

Connect all ESP8266 devices to the same OneWire bus:

```
ESP8266 #1 (Master)    ESP8266 #2 (Slave ID:1)    ESP8266 #3 (Slave ID:2)
       |                       |                           |
       +-------[4.7kΩ]---------+                           |
       |                       |                           |
     GPIO2                   GPIO2                       GPIO2
       |                       |                           |
       +-------+---------------+---------------------------+  (OneWire Bus)
       |       |               |                           |
      GND     3.3V            GND                         GND
       |       |               |                           |
       +-------+---------------+---------------------------+  (Common Ground)
               |
               +  (Pull-up resistor to 3.3V)
```

**Important**: Only ONE 4.7kΩ pull-up resistor is needed for the entire bus!

### Upload Process

1. **Flash Master Device**:
   - Set `DEVICE_MODE_MASTER true`
   - Upload to first ESP8266

2. **Flash Slave Device 1**:
   - Set `DEVICE_MODE_MASTER false` and `SLAVE_ID 1`
   - Upload to second ESP8266

3. **Flash Slave Device 2**:
   - Set `DEVICE_MODE_MASTER false` and `SLAVE_ID 2`
   - Upload to third ESP8266

4. **And so on...**

### Expected Behavior

**Master Device**:
- Sends heartbeat to slave ID 1, then 2, then 3, etc.
- Cycles through all slave IDs (1 to 5) every 3 seconds
- LED blinks briefly when sending each heartbeat
- Serial output shows which slave ID is being addressed

**Slave Device ID 1**:
- Listens for heartbeats
- When it receives a heartbeat for ID 1, blinks LED **1 time**
- Ignores heartbeats for other IDs
- Serial output shows received heartbeats and responses

**Slave Device ID 2**:
- Listens for heartbeats
- When it receives a heartbeat for ID 2, blinks LED **2 times**
- Ignores heartbeats for other IDs

**Slave Device ID 3**:
- Listens for heartbeats
- When it receives a heartbeat for ID 3, blinks LED **3 times**
- Ignores heartbeats for other IDs

### Serial Monitor Output Examples

**Master Device**:
```
=== OneWire ESP8266 Master-Slave Communication ===
*** DEVICE CONFIGURED AS MASTER ***
Will send heartbeats to slaves 1-5
Heartbeat interval: 3000 ms

=== Sending Heartbeat to Slave ID: 1 ===
Heartbeat sent with ID: 1

=== Sending Heartbeat to Slave ID: 2 ===
Heartbeat sent with ID: 2

=== Sending Heartbeat to Slave ID: 3 ===
Heartbeat sent with ID: 3
```

**Slave Device (ID 2)**:
```
=== OneWire ESP8266 Master-Slave Communication ===
*** DEVICE CONFIGURED AS SLAVE ***
My Slave ID: 2
Will blink 2 times when my ID is called

Received heartbeat for ID: 1 (My ID: 2)
Heartbeat for different slave, ignoring

Received heartbeat for ID: 2 (My ID: 2)
*** This heartbeat is for ME! ***
Blinking 2 times for my ID
```

### Troubleshooting

**No Communication**:
- Check that all devices share common ground
- Verify 4.7kΩ pull-up resistor is connected
- Ensure only one device is configured as master
- Check GPIO2 connections on all devices

**Slaves Don't Respond**:
- Verify slave IDs are set correctly (1, 2, 3, etc.)
- Check that `DEVICE_MODE_MASTER` is set to `false` for slaves
- Monitor serial output to see if heartbeats are being received

**Multiple Responses**:
- Make sure each slave has a unique ID
- Verify no two devices have the same configuration

### Scaling Up

To add more slaves:
1. Increase `MAX_SLAVES` in the code
2. Set unique `SLAVE_ID` for each new slave device
3. Flash and connect to the same OneWire bus

The system supports up to 10+ devices on the same bus with proper power supply.
