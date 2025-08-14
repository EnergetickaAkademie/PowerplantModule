#!/usr/bin/env python3

"""
PJON Slave Configuration Generator
This script generates PlatformIO environments for multiple PJON slaves
"""

import argparse
import os
import sys
from pathlib import Path

def generate_slave_configs(start_id, end_id, slave_type=1, output_file=None):
    """Generate slave configurations for platformio.ini"""
    
    configs = []
    
    for slave_id in range(start_id, end_id + 1):
        # USB flash environment
        usb_config = f"""
[env:slave_{slave_id}]
extends = env
build_flags = 
    -D SLAVE_ID={slave_id}
    -D SLAVE_TYPE={slave_type}
    -D DEVICE_NAME=\\"PjonSlave{slave_id}\\"
"""
        
        # OTA environment
        ota_config = f"""
[env:slave_{slave_id}_ota]
extends = env:slave_{slave_id}
upload_protocol = espota
upload_port = PjonSlave{slave_id}.local
upload_flags = --auth=admin
"""
        
        configs.append(usb_config + ota_config)
    
    config_text = "\n".join(configs)
    
    if output_file:
        with open(output_file, 'w') as f:
            f.write(config_text)
        print(f"Generated configurations for slaves {start_id}-{end_id} in {output_file}")
    else:
        print(config_text)

def update_platformio_ini(start_id, end_id, slave_type=1):
    """Update existing platformio.ini with new slave configurations"""
    
    platformio_file = Path("platformio.ini")
    
    if not platformio_file.exists():
        print("Error: platformio.ini not found in current directory")
        sys.exit(1)
    
    # Read existing content
    with open(platformio_file, 'r') as f:
        content = f.read()
    
    # Generate new configurations
    new_configs = []
    
    for slave_id in range(start_id, end_id + 1):
        # Check if configuration already exists
        if f"[env:slave_{slave_id}]" in content:
            print(f"Warning: Configuration for slave {slave_id} already exists, skipping...")
            continue
        
        usb_config = f"""
[env:slave_{slave_id}]
extends = env
build_flags = 
    -D SLAVE_ID={slave_id}
    -D SLAVE_TYPE={slave_type}
    -D DEVICE_NAME=\\"PjonSlave{slave_id}\\"

[env:slave_{slave_id}_ota]
extends = env:slave_{slave_id}
upload_protocol = espota
upload_port = PjonSlave{slave_id}.local
upload_flags = --auth=admin
"""
        new_configs.append(usb_config)
    
    if new_configs:
        # Append new configurations
        with open(platformio_file, 'a') as f:
            f.write("\n" + "\n".join(new_configs))
        
        print(f"Added configurations for slaves {start_id}-{end_id} to platformio.ini")
    else:
        print("No new configurations added (all already exist)")

def list_existing_slaves():
    """List existing slave configurations in platformio.ini"""
    
    platformio_file = Path("platformio.ini")
    
    if not platformio_file.exists():
        print("Error: platformio.ini not found in current directory")
        sys.exit(1)
    
    with open(platformio_file, 'r') as f:
        content = f.read()
    
    import re
    
    # Find all slave configurations
    slaves = re.findall(r'\[env:slave_(\d+)\]', content)
    
    if slaves:
        print("Existing slave configurations:")
        for slave_id in sorted(set(slaves), key=int):
            # Try to extract slave type
            pattern = rf'\[env:slave_{slave_id}\].*?SLAVE_TYPE=(\d+)'
            match = re.search(pattern, content, re.DOTALL)
            slave_type = match.group(1) if match else "unknown"
            
            print(f"  Slave {slave_id} (Type: {slave_type})")
    else:
        print("No existing slave configurations found")

def generate_flash_script(slave_ids):
    """Generate a simple bash script for flashing specific slaves"""
    
    script_content = f"""#!/bin/bash

# Auto-generated flash script for slaves: {', '.join(map(str, slave_ids))}

set -e

echo "PJON Slave Flash Script"
echo "======================="
echo ""

"""

    for slave_id in slave_ids:
        script_content += f"""
echo "Flashing Slave {slave_id}..."
echo "Connect ESP8266 for slave {slave_id} and press Enter"
read -p "Ready? " 
pio run -e slave_{slave_id} -t upload
echo "Slave {slave_id} flashed successfully!"
echo ""
"""
    
    script_content += """
echo "All slaves flashed successfully!"
echo "Slaves are now available for OTA updates at:"
"""
    
    for slave_id in slave_ids:
        script_content += f'echo "  Slave {slave_id}: PjonSlave{slave_id}.local"\n'
    
    with open("flash_selected_slaves.sh", 'w') as f:
        f.write(script_content)
    
    os.chmod("flash_selected_slaves.sh", 0o755)
    print(f"Generated flash_selected_slaves.sh for slaves: {', '.join(map(str, slave_ids))}")

def main():
    parser = argparse.ArgumentParser(description="PJON Slave Configuration Generator")
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Generate command
    gen_parser = subparsers.add_parser('generate', help='Generate slave configurations')
    gen_parser.add_argument('start_id', type=int, help='Starting slave ID')
    gen_parser.add_argument('end_id', type=int, help='Ending slave ID')
    gen_parser.add_argument('--type', type=int, default=1, help='Slave type (default: 1)')
    gen_parser.add_argument('--output', help='Output file (default: stdout)')
    
    # Update command
    update_parser = subparsers.add_parser('update', help='Update platformio.ini with new slaves')
    update_parser.add_argument('start_id', type=int, help='Starting slave ID')
    update_parser.add_argument('end_id', type=int, help='Ending slave ID')
    update_parser.add_argument('--type', type=int, default=1, help='Slave type (default: 1)')
    
    # List command
    subparsers.add_parser('list', help='List existing slave configurations')
    
    # Flash script command
    flash_parser = subparsers.add_parser('flash-script', help='Generate flash script for specific slaves')
    flash_parser.add_argument('slave_ids', nargs='+', type=int, help='Slave IDs to include in flash script')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        sys.exit(1)
    
    if args.command == 'generate':
        generate_slave_configs(args.start_id, args.end_id, args.type, args.output)
    elif args.command == 'update':
        update_platformio_ini(args.start_id, args.end_id, args.type)
    elif args.command == 'list':
        list_existing_slaves()
    elif args.command == 'flash-script':
        generate_flash_script(args.slave_ids)

if __name__ == '__main__':
    main()
