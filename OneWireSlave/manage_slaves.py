#!/usr/bin/env python3

"""
PJON Slave Management Script
This script handles initial USB flashing and OTA updates for multiple slaves
"""

import os
import sys
import subprocess
import re
import argparse
import time
import socket
import threading
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from typing import List, Dict, Optional, Tuple

class Colors:
    """ANSI color codes for terminal output"""
    RED = '\033[0;31m'
    GREEN = '\033[0;32m'
    YELLOW = '\033[1;33m'
    BLUE = '\033[0;34m'
    PURPLE = '\033[0;35m'
    CYAN = '\033[0;36m'
    WHITE = '\033[1;37m'
    NC = '\033[0m'  # No Color

class SlaveInfo:
    """Information about a PJON slave"""
    def __init__(self, slave_id: int, slave_type: int = None, device_name: str = None):
        self.id = slave_id
        self.type = slave_type or 1
        self.device_name = device_name or f"PjonSlave{slave_id}"
        self.hostname = f"{self.device_name}.local"
    
    def __str__(self):
        return f"Slave {self.id} (Type: {self.type}, Name: {self.device_name})"

class SlaveManager:
    """Main class for managing PJON slaves"""
    
    def __init__(self):
        self.platformio_file = Path("platformio.ini")
        self.check_directory()
    
    def check_directory(self):
        """Check if we're in the right directory"""
        if not self.platformio_file.exists():
            self.print_error("platformio.ini not found. Please run this script from the OneWireSlave directory.")
            sys.exit(1)
    
    def print_status(self, message: str):
        print(f"{Colors.BLUE}[INFO]{Colors.NC} {message}")
    
    def print_success(self, message: str):
        print(f"{Colors.GREEN}[SUCCESS]{Colors.NC} {message}")
    
    def print_warning(self, message: str):
        print(f"{Colors.YELLOW}[WARNING]{Colors.NC} {message}")
    
    def print_error(self, message: str):
        print(f"{Colors.RED}[ERROR]{Colors.NC} {message}")
    
    def run_command(self, cmd: List[str], capture_output: bool = False, timeout: int = 600) -> Tuple[bool, str]:
        """Run a command and return success status and output"""
        try:
            if capture_output:
                result = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
                return result.returncode == 0, result.stdout + result.stderr
            else:
                result = subprocess.run(cmd, timeout=timeout)
                return result.returncode == 0, ""
        except subprocess.TimeoutExpired:
            return False, "Command timed out"
        except Exception as e:
            return False, str(e)
    
    def parse_platformio_ini(self) -> Dict[int, SlaveInfo]:
        """Parse platformio.ini and extract slave configurations"""
        slaves = {}
        
        with open(self.platformio_file, 'r') as f:
            content = f.read()
        
        # Find all slave environments
        env_pattern = r'\[env:slave_(\d+)\]'
        env_matches = re.finditer(env_pattern, content)
        
        for match in env_matches:
            slave_id = int(match.group(1))
            
            # Extract the environment section
            start_pos = match.start()
            
            # Find the end of this environment (next [env:] or end of file)
            next_env_match = re.search(r'\n\[env:', content[start_pos + 1:])
            if next_env_match:
                end_pos = start_pos + 1 + next_env_match.start()
                env_content = content[start_pos:end_pos]
            else:
                env_content = content[start_pos:]
            
            # Extract slave type and device name
            slave_type = 1  # default
            device_name = f"PjonSlave{slave_id}"  # default
            
            type_match = re.search(r'-D SLAVE_TYPE=(\d+)', env_content)
            if type_match:
                slave_type = int(type_match.group(1))
            
            name_match = re.search(r'-D DEVICE_NAME=\\"([^"]+)\\"', env_content)
            if name_match:
                device_name = name_match.group(1)
            
            slaves[slave_id] = SlaveInfo(slave_id, slave_type, device_name)
        
        return slaves
    
    def slave_exists(self, slave_id: int) -> bool:
        """Check if slave configuration exists"""
        slaves = self.parse_platformio_ini()
        return slave_id in slaves
    
    def is_slave_online(self, hostname: str, timeout: int = 1) -> bool:
        """Check if slave is reachable via network"""
        try:
            # Try to resolve hostname with shorter timeout
            socket.setdefaulttimeout(timeout)
            socket.gethostbyname(hostname)
            
            # Try to ping with shorter timeout
            result = subprocess.run(['ping', '-c', '1', '-W', str(timeout), hostname], 
                                 capture_output=True, timeout=timeout + 0.5)
            return result.returncode == 0
        except:
            return False
        finally:
            socket.setdefaulttimeout(None)  # Reset to default
    
    def flash_slave(self, slave_id: int) -> bool:
        """Flash slave via USB"""
        if not self.slave_exists(slave_id):
            self.print_error(f"Slave {slave_id} configuration not found in platformio.ini")
            return False
        
        self.print_status(f"Flashing slave {slave_id} via USB...")
        self.print_warning("Make sure the ESP8266 is connected via USB and in programming mode")
        
        success, output = self.run_command(['pio', 'run', '-e', f'slave_{slave_id}', '-t', 'upload'])
        
        if success:
            self.print_success(f"Slave {slave_id} flashed successfully!")
            self.print_status(f"The slave will now be available as PjonSlave{slave_id}.local for OTA updates")
            return True
        else:
            self.print_error(f"Failed to flash slave {slave_id}")
            if output:
                print(output)
            return False
    
    def ota_slave(self, slave_id: int) -> bool:
        """Update slave via OTA"""
        if not self.slave_exists(slave_id):
            self.print_error(f"Slave {slave_id} configuration not found in platformio.ini")
            return False
        
        slaves = self.parse_platformio_ini()
        slave = slaves[slave_id]
        
        self.print_status(f"Updating slave {slave_id} via OTA...")
        self.print_status(f"Trying to reach {slave.hostname}...")
        
        if self.is_slave_online(slave.hostname):
            self.print_success(f"Slave {slave_id} is reachable")
            
            success, output = self.run_command(['pio', 'run', '-e', f'slave_{slave_id}_ota', '-t', 'upload'])
            
            if success:
                self.print_success(f"Slave {slave_id} updated successfully via OTA!")
                return True
            else:
                self.print_error(f"Failed to update slave {slave_id} via OTA")
                self.print_warning("Try: 1) Check if device is online, 2) Flash via USB if needed")
                if output:
                    print(output)
                return False
        else:
            self.print_error(f"Cannot reach {slave.hostname}")
            self.print_warning("Make sure the slave is powered on and connected to WiFi")
            self.print_warning(f"If this is the first time, use: {sys.argv[0]} flash {slave_id}")
            return False
    
    def check_slave_status(self, slave: SlaveInfo) -> Tuple[int, bool]:
        """Check if a single slave is online (for parallel execution)"""
        return slave.id, self.is_slave_online(slave.hostname)

    def check_slaves_status_parallel(self, slaves: Dict[int, SlaveInfo]) -> Dict[int, bool]:
        """Check status of all slaves in parallel"""
        status = {}
        
        # Use ThreadPoolExecutor for parallel network checks
        with ThreadPoolExecutor(max_workers=min(len(slaves), 10)) as executor:
            # Submit all tasks
            future_to_slave = {
                executor.submit(self.check_slave_status, slave): slave_id 
                for slave_id, slave in slaves.items()
            }
            
            # Collect results as they complete, with individual timeouts handled per task
            for future in as_completed(future_to_slave):
                try:
                    slave_id, is_online = future.result(timeout=2)
                    status[slave_id] = is_online
                except Exception:
                    # If a check fails, mark as offline
                    slave_id = future_to_slave[future]
                    status[slave_id] = False
        
        return status

    def list_slaves(self):
        """List all configured slaves"""
        slaves = self.parse_platformio_ini()
        
        if not slaves:
            self.print_warning("No slave configurations found in platformio.ini")
            return
        
        self.print_status("Configured slaves:")
        self.print_status("Checking status in parallel...")
        
        # Check all slaves status in parallel
        status = self.check_slaves_status_parallel(slaves)
        
        print()
        
        for slave_id in sorted(slaves.keys()):
            slave = slaves[slave_id]
            is_online = status.get(slave_id, False)
            
            print(f"  Slave {slave_id}:")
            print(f"    Type: {slave.type}")
            print(f"    Device Name: {slave.device_name}")
            print(f"    OTA Address: {slave.hostname}")
            
            # Show status with color
            if is_online:
                print(f"    Status: {Colors.GREEN}Online{Colors.NC}")
            else:
                print(f"    Status: {Colors.RED}Offline{Colors.NC}")
            print()
    
    def monitor_slave(self, slave_id: int):
        """Monitor slave serial output"""
        if not self.slave_exists(slave_id):
            self.print_error(f"Slave {slave_id} configuration not found in platformio.ini")
            return False
        
        self.print_status(f"Monitoring slave {slave_id} serial output...")
        self.print_warning("Press Ctrl+C to exit monitoring")
        
        try:
            subprocess.run(['pio', 'device', 'monitor', '-e', f'slave_{slave_id}'])
            return True
        except KeyboardInterrupt:
            print("\nMonitoring stopped.")
            return True
        except Exception as e:
            self.print_error(f"Failed to monitor slave {slave_id}: {e}")
            return False
    
    def ping_slave(self, slave_id: int) -> bool:
        """Ping slave"""
        if not self.slave_exists(slave_id):
            self.print_error(f"Slave {slave_id} configuration not found in platformio.ini")
            return False
        
        slaves = self.parse_platformio_ini()
        slave = slaves[slave_id]
        
        self.print_status(f"Pinging {slave.hostname}...")
        
        success, output = self.run_command(['ping', '-c', '4', slave.hostname], capture_output=True)
        
        if success:
            self.print_success(f"Slave {slave_id} is reachable!")
            return True
        else:
            self.print_error(f"Slave {slave_id} is not reachable")
            return False
    
    def flash_all(self):
        """Flash all slaves via USB"""
        slaves = self.parse_platformio_ini()
        
        if not slaves:
            self.print_warning("No slave configurations found")
            return
        
        self.print_status("Flashing all configured slaves via USB...")
        self.print_warning("You'll need to connect each ESP8266 one by one")
        
        for slave_id in sorted(slaves.keys()):
            self.print_status(f"Ready to flash slave {slave_id}")
            
            try:
                response = input(f"Connect slave {slave_id} and press Enter to continue (or 's' to skip): ")
                
                if response.lower() == 's':
                    self.print_warning(f"Skipping slave {slave_id}")
                    continue
                
                if self.flash_slave(slave_id):
                    self.print_success(f"Slave {slave_id} completed. Disconnect and prepare next slave.")
                else:
                    self.print_error(f"Failed to flash slave {slave_id}")
                print()
                
            except KeyboardInterrupt:
                print("\nFlashing interrupted.")
                break
        
        self.print_success("Flash operation completed!")
    
    def ota_all(self):
        """OTA update all slaves"""
        slaves = self.parse_platformio_ini()
        
        if not slaves:
            self.print_warning("No slave configurations found")
            return
        
        self.print_status("Checking which slaves are online...")
        
        # Check all slaves status in parallel first
        status = self.check_slaves_status_parallel(slaves)
        
        self.print_status("Updating all reachable slaves via OTA...")
        
        updated = 0
        failed = 0
        
        for slave_id in sorted(slaves.keys()):
            slave = slaves[slave_id]
            is_online = status.get(slave_id, False)
            
            if is_online:
                self.print_status(f"Updating slave {slave_id}...")
                if self.ota_slave(slave_id):
                    updated += 1
                else:
                    failed += 1
            else:
                self.print_warning(f"Slave {slave_id} is offline, skipping...")
            print()
        
        self.print_status("OTA update summary:")
        self.print_success(f"{updated} slaves updated successfully")
        if failed > 0:
            self.print_error(f"{failed} slaves failed to update")
    
    def build_slave(self, slave_id: int) -> bool:
        """Build slave firmware without uploading"""
        if not self.slave_exists(slave_id):
            self.print_error(f"Slave {slave_id} configuration not found in platformio.ini")
            return False
        
        self.print_status(f"Building slave {slave_id} firmware...")
        
        success, output = self.run_command(['pio', 'run', '-e', f'slave_{slave_id}'], capture_output=True)
        
        if success:
            self.print_success(f"Slave {slave_id} built successfully!")
            return True
        else:
            self.print_error(f"Failed to build slave {slave_id}")
            if output:
                print(output)
            return False
    
    def build_all(self):
        """Build all slaves"""
        slaves = self.parse_platformio_ini()
        
        if not slaves:
            self.print_warning("No slave configurations found")
            return
        
        self.print_status("Building all slave configurations...")
        
        built = 0
        failed = 0
        
        for slave_id in sorted(slaves.keys()):
            if self.build_slave(slave_id):
                built += 1
            else:
                failed += 1
        
        self.print_status("Build summary:")
        self.print_success(f"{built} slaves built successfully")
        if failed > 0:
            self.print_error(f"{failed} slaves failed to build")
    
    def show_usage(self):
        """Show usage information"""
        print("PJON Slave Management Script")
        print()
        print(f"Usage: {sys.argv[0]} <command> [slave_id]")
        print()
        print("Commands:")
        print("  flash <id>     - Flash slave via USB (first time)")
        print("  ota <id>       - Update slave via OTA")
        print("  list           - List all configured slaves")
        print("  monitor <id>   - Monitor slave serial output")
        print("  ping <id>      - Check if slave is reachable via network")
        print("  build <id>     - Build slave firmware (no upload)")
        print("  flash-all      - Flash all slaves via USB")
        print("  ota-all        - Update all slaves via OTA")
        print("  build-all      - Build all slave configurations")
        print()
        print("Examples:")
        print(f"  {sys.argv[0]} flash 10           # Flash slave 10 via USB")
        print(f"  {sys.argv[0]} ota 10             # Update slave 10 via OTA")
        print(f"  {sys.argv[0]} monitor 10         # Monitor slave 10")
        print(f"  {sys.argv[0]} ping 10            # Ping PjonSlave10.local")
        print()

def main():
    manager = SlaveManager()
    
    if len(sys.argv) < 2:
        manager.show_usage()
        sys.exit(1)
    
    command = sys.argv[1].lower()
    
    if command in ['flash', 'ota', 'monitor', 'ping', 'build']:
        if len(sys.argv) < 3:
            manager.print_error("Please specify slave ID")
            manager.show_usage()
            sys.exit(1)
        
        try:
            slave_id = int(sys.argv[2])
        except ValueError:
            manager.print_error("Slave ID must be a number")
            sys.exit(1)
        
        if command == 'flash':
            success = manager.flash_slave(slave_id)
        elif command == 'ota':
            success = manager.ota_slave(slave_id)
        elif command == 'monitor':
            success = manager.monitor_slave(slave_id)
        elif command == 'ping':
            success = manager.ping_slave(slave_id)
        elif command == 'build':
            success = manager.build_slave(slave_id)
        
        sys.exit(0 if success else 1)
    
    elif command == 'list':
        manager.list_slaves()
    
    elif command == 'flash-all':
        manager.flash_all()
    
    elif command == 'ota-all':
        manager.ota_all()
    
    elif command == 'build-all':
        manager.build_all()
    
    else:
        manager.print_error(f"Unknown command: {command}")
        manager.show_usage()
        sys.exit(1)

if __name__ == '__main__':
    main()
