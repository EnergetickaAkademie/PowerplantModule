#!/bin/bash

# PJON Slave Flash Script
# Usage: ./flash_slave.sh <slave_id> [ota]
# Example: ./flash_slave.sh 10        (flash via USB)
# Example: ./flash_slave.sh 10 ota    (flash via OTA)

if [ $# -eq 0 ]; then
    echo "Usage: $0 <slave_id> [ota]"
    echo ""
    echo "Available slave configurations:"
    echo "  10-14: Pre-configured slaves"
    echo ""
    echo "Examples:"
    echo "  $0 10      # Flash slave 10 via USB"
    echo "  $0 10 ota  # Flash slave 10 via OTA"
    echo "  $0 12      # Flash slave 12 via USB"
    exit 1
fi

SLAVE_ID=$1
USE_OTA=$2

# Determine environment name
if [ "$USE_OTA" = "ota" ]; then
    ENV="slave_${SLAVE_ID}_ota"
    echo "🌐 Flashing Slave $SLAVE_ID via OTA..."
else
    ENV="slave_${SLAVE_ID}"
    echo "🔌 Flashing Slave $SLAVE_ID via USB..."
fi

echo "Environment: $ENV"
echo ""

# Change to the OneWireSlave directory
cd "$(dirname "$0")"

# Run PlatformIO
pio run -e $ENV --target upload

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Slave $SLAVE_ID flashed successfully!"
    echo ""
    echo "Device configuration:"
    echo "  - Slave ID: $SLAVE_ID"
    echo "  - Device Name: PjonSlave$SLAVE_ID"
    if [ "$USE_OTA" = "ota" ]; then
        echo "  - OTA Address: PjonSlave$SLAVE_ID.local"
    fi
else
    echo ""
    echo "❌ Failed to flash Slave $SLAVE_ID"
    exit 1
fi
