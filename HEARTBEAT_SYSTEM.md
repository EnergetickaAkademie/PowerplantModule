# PJON Slave Discovery and Heartbeat System

## Overview
This system implements automatic slave discovery and heartbeat monitoring using PJON communication protocol. The master tracks connected slaves using a dynamic data structure and automatically removes them from the active list when they disconnect.

## System Behavior

### Master (OneWireHost)
- **Dynamic Slave Tracking**: Uses `std::vector` for unlimited slave capacity
- **Idle State**: Master waits for slave heartbeats
- **Slave Discovery**: When a slave connects and sends its first heartbeat, master adds it to the active slaves list
- **Heartbeat Monitoring**: Master expects heartbeats every second from each slave
- **Timeout Detection**: If no heartbeat received for 2 seconds, slave is automatically removed
- **Status Reporting**: Active slaves list is printed to WebSerial every second

### Slave (OneWireSlave)
- **Heartbeat Transmission**: Sends heartbeat every 1 second containing:
  - Message type (HEARTBEAT = 0x03)
  - Slave ID (8-bit unsigned integer)
  - Slave type (8-bit unsigned integer)
- **Automatic Registration**: Master automatically discovers and tracks the slave upon first heartbeat

## Configuration

### Master Configuration
```cpp
#define HEARTBEAT_TIMEOUT_MS 2000  // 2 seconds timeout
// No maximum slave limit - uses dynamic vector
```

### Slave Configuration
```cpp
#define SLAVE_ID 10               // Change for each slave (10, 11, 12, etc.)
#define SLAVE_TYPE 1              // Define slave type (1=temp sensor, 2=relay, etc.)
#define HEARTBEAT_INTERVAL 1000   // Send heartbeat every 1 second
```

## Message Protocol

### Heartbeat Message Format
| Byte 0 | Byte 1 | Byte 2 |
|--------|--------|--------|
| 0x03   | Slave ID | Slave Type |
| (HEARTBEAT) | (8-bit) | (8-bit) |

## Data Structures

### Master Slave Tracking (Dynamic)
```cpp
struct SlaveInfo {
    uint8_t id;                    // Slave ID
    uint8_t type;                  // Slave type
    unsigned long lastHeartbeat;   // Last heartbeat timestamp
    
    // Constructor for easy initialization
    SlaveInfo(uint8_t _id, uint8_t _type) : id(_id), type(_type), lastHeartbeat(millis()) {}
};

std::vector<SlaveInfo> slaves;     // Dynamic array of slaves
```

## Key Features

1. **Unlimited Capacity**: No predefined maximum number of slaves
2. **Memory Efficient**: Only allocates memory for connected slaves
3. **Automatic Discovery**: No need to pre-configure slave IDs in master
4. **Dynamic Management**: Slaves can connect/disconnect at any time
5. **Timeout Detection**: Automatic removal of disconnected slaves
6. **Type Classification**: Each slave can have a different type identifier
7. **Real-time Monitoring**: WebSerial displays current active slaves
8. **Robust Communication**: Uses PJON's built-in acknowledgment and error handling

## Memory Management

- **Dynamic Allocation**: Uses `std::vector` for automatic memory management
- **Reserved Capacity**: Pre-reserves space for 20 slaves to minimize reallocations
- **Automatic Cleanup**: Slaves are automatically removed when they timeout
- **Iterator Safety**: Uses safe iteration when removing timed-out slaves

## WebSerial Output Example
```
Active slaves (2): ID:10,Type:1 ID:11,Type:2
New slave connected - ID 12, type 1
Active slaves (3): ID:10,Type:1 ID:11,Type:2 ID:12,Type:1
Slave ID 11 disconnected (timeout)
Active slaves (2): ID:10,Type:1 ID:12,Type:1
```

## Usage

1. **Flash Master**: Upload OneWireHost code to master device
2. **Flash Slaves**: Upload OneWireSlave code to slave devices (set unique SLAVE_ID for each)
3. **Connect Hardware**: Wire slaves to master via PJON bus (pin D1)
4. **Monitor**: Access WebSerial to see real-time slave status
5. **Hot-plug Support**: Slaves can be connected/disconnected during operation
6. **Scalability**: Connect as many slaves as needed (limited only by available memory)

## Slave Types
Define different slave types using the SLAVE_TYPE constant:
- `1`: Temperature sensor
- `2`: Relay controller  
- `3`: Digital input
- `4`: Analog sensor
- etc.

This allows the master to identify and handle different slave capabilities appropriately.

## Technical Improvements

### From Fixed Array to Dynamic Vector
- **Before**: Fixed array `SlaveInfo slaves[MAX_SLAVES]` with manual tracking
- **After**: Dynamic `std::vector<SlaveInfo> slaves` with automatic memory management
- **Benefits**:
  - No artificial limit on slave count
  - Memory only used for active slaves
  - Automatic cleanup and resizing
  - Modern C++ iterator-based operations
  - Safe element removal during iteration
