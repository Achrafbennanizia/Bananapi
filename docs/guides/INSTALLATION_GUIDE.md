# Complete Installation Guide for Wallbox Controller

## 📝 Placeholder Substitution Guide

Throughout this guide, you'll see placeholders that need to be replaced with your actual values:

| Placeholder      | Description                                                | Example                      |
| ---------------- | ---------------------------------------------------------- | ---------------------------- |
| `<API_HOST>`     | IP address of your target device (Banana Pi, Raspberry Pi) | `192.168.1.100`              |
| `<SIM_HOST>`     | IP address where simulator runs (usually localhost)        | `localhost` or `127.0.0.1`   |
| `<PROJECT_ROOT>` | Full path to project directory on your machine             | `/home/user/wallbox-project` |
| `<PI_USER>`      | Username on target device                                  | `pi`, `root`, `bananapi`     |

**Example substitution:**

```bash
# Documentation shows:
ssh <PI_USER>@<API_HOST>

# You would use:
ssh pi@192.168.1.100
```

---

## Overview

This guide provides detailed instructions for installing, building, and deploying the Wallbox Controller system on any ARM-based single-board computer (Raspberry Pi, Banana Pi, Orange Pi, etc.).

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Hardware Requirements](#hardware-requirements)
3. [Initial Pi Setup](#initial-pi-setup)
4. [Software Installation](#software-installation)
5. [Project Setup](#project-setup)
6. [Building the Project](#building-the-project)
7. [Configuration](#configuration)
8. [Running the Wallbox](#running-the-wallbox)
9. [Testing](#testing)
10. [Production Deployment](#production-deployment)
11. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Required Knowledge

- Basic Linux command line usage
- SSH access to single-board computer
- Basic understanding of networking (IP addresses, ports)
- Text editor usage (nano, vim, or local editor)

### Required Tools (on Development Machine)

- SSH client (OpenSSH, PuTTY, etc.)
- SCP/SFTP client for file transfer
- Terminal/Command prompt
- Text editor

### Network Requirements

- Pi and development machine on same network
- Static IP recommended for Pi
- Ports required:
  - **SSH**: 22 (for remote access)
  - **HTTP API**: 8080 (for REST API)
  - **UDP**: 50010, 50011 (for ISO 15118 communication)

---

## Hardware Requirements

### Supported Platforms

- ✅ Raspberry Pi (all models with GPIO)
- ✅ Banana Pi (M1, M2, M3, M4, M5)
- ✅ Orange Pi
- ✅ Any ARM-based SBC with GPIO support

### Minimum Specifications

- **CPU**: ARM Cortex-A7 or newer
- **RAM**: 512 MB minimum, 1 GB recommended
- **Storage**: 4 GB minimum, 8 GB recommended
- **Network**: Ethernet or WiFi
- **GPIO**: At least 5 GPIO pins available

### GPIO Pin Requirements

| Pin Purpose    | Quantity | Notes                          |
| -------------- | -------- | ------------------------------ |
| Relay Control  | 1        | Main contactor control         |
| LED Indicators | 3        | Green, Yellow, Red status LEDs |
| Button Input   | 1        | Optional physical control      |

### Optional Hardware

- Real-time clock (RTC) module
- LCD display for status
- Buzzer for alerts
- Temperature sensor

---

## Initial Pi Setup

### 1. Install Operating System

#### For Raspberry Pi

```bash
# Download Raspberry Pi OS Lite (recommended)
# URL: https://www.raspberrypi.com/software/

# Flash to SD card using Raspberry Pi Imager
# or use dd command:
sudo dd if=2024-07-04-raspios-bookworm-arm64-lite.img of=/dev/sdX bs=4M status=progress
```

#### For Banana Pi

```bash
# Download Armbian or Debian for Banana Pi
# URL: https://www.armbian.com/banana-pi/

# Flash to SD card
sudo dd if=Armbian_*.img of=/dev/sdX bs=4M status=progress
```

### 2. First Boot Configuration

```bash
# SSH into your Pi
ssh pi@<SSH_TARGET_SUBNET>
# Default password: raspberry (Raspberry Pi) or 1234 (Armbian)

# Change default password immediately
passwd

# Update system
sudo apt update
sudo apt upgrade -y

# Set hostname (optional but recommended)
sudo hostnamectl set-hostname wallbox-pi

# Reboot
sudo reboot
```

### 3. Configure Static IP (Recommended)

```bash
# Edit dhcpcd configuration
sudo nano /etc/dhcpcd.conf

# Add at the end (adjust to match your network):
interface eth0
static ip_address=<API_HOST>/24
static routers=<GATEWAY_IP>
static domain_name_servers=<GATEWAY_IP> 8.8.8.8

# Save and exit (Ctrl+X, Y, Enter)

# Reboot to apply
sudo reboot
```

### 4. Enable SSH (if not enabled)

```bash
# Raspberry Pi
sudo systemctl enable ssh
sudo systemctl start ssh

# Verify
sudo systemctl status ssh
```

### 5. Configure Firewall

```bash
# Install firewall
sudo apt install ufw -y

# Allow SSH
sudo ufw allow 22/tcp

# Allow HTTP API
sudo ufw allow 8080/tcp

# Allow UDP communication
sudo ufw allow 50010/udp
sudo ufw allow 50011/udp

# Enable firewall
sudo ufw enable

# Check status
sudo ufw status
```

---

## Software Installation

### 1. Install Build Tools

```bash
# Update package list
sudo apt update

# Install essential build tools
sudo apt install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    autoconf \
    automake \
    libtool

# Install GCC/G++ (if not already installed)
sudo apt install -y gcc g++

# Verify installation
gcc --version
g++ --version
cmake --version
```

### 2. Install Required Libraries

```bash
# Install libmicrohttpd (for HTTP API server)
sudo apt install -y libmicrohttpd-dev

# Install libcurl (for HTTP client functionality)
sudo apt install -y libcurl4-openssl-dev

# Install JSON parsing library
sudo apt install -y nlohmann-json3-dev

# Install GPIO libraries (Raspberry Pi specific)
# For Raspberry Pi:
sudo apt install -y wiringpi

# For Banana Pi:
sudo apt install -y wiringpi-bananapi
# Or build from source if not available in repos

# Install additional utilities
sudo apt install -y \
    net-tools \
    lsof \
    htop \
    vim \
    screen
```

### 3. Install Optional Dependencies

```bash
# Install systemd development files (for service creation)
sudo apt install -y libsystemd-dev

# Install documentation tools
sudo apt install -y doxygen graphviz

# Install testing tools
sudo apt install -y valgrind gdb
```

---

## Project Setup

### 1. Create Project Directory

```bash
# Create workspace
mkdir -p ~/wallbox-project
cd ~/wallbox-project

# Create directory structure
mkdir -p src build include lib logs
```

### 2. Transfer Project Files

#### Option A: Using Git (Recommended)

```bash
# Clone repository
cd ~/wallbox-project
git clone https://github.com/yourusername/wallbox-controller.git .

# Or if using a specific branch
git clone -b main https://github.com/yourusername/wallbox-controller.git .
```

#### Option B: Using SCP (from Development Machine)

```bash
# On your development machine (Mac/Linux)
cd /path/to/your/project

# Copy entire WallboxCtrl directory
scp -r WallboxCtrl pi@<API_HOST>:~/wallbox-project/

# Or copy specific directories
scp -r WallboxCtrl/src pi@<API_HOST>:~/wallbox-project/
scp -r WallboxCtrl/include pi@<API_HOST>:~/wallbox-project/
scp -r build/bin pi@<API_HOST>:~/wallbox-project/
```

#### Option C: Manual File Creation

```bash
# Create each file manually if network transfer is not available
# Use nano or vim to create files

# Example:
nano ~/wallbox-project/src/main_v3.cpp
# Paste content and save
```

### 3. Verify File Structure

```bash
cd ~/wallbox-project
tree -L 2

# Expected structure:
# wallbox-project/
# ├── build/
# │   ├── CMakeLists.txt
# │   └── config.json
# ├── include/
# │   ├── Application.h
# │   ├── WallboxController.h
# │   ├── Configuration.h
# │   └── ...
# └── src/
#     ├── main_v3.cpp
#     ├── WallboxController.cpp
#     └── ...
```

### 4. Set Correct Permissions

```bash
cd ~/wallbox-project

# Make build scripts executable
chmod +x build/*.sh 2>/dev/null || true

# Set ownership
sudo chown -R $USER:$USER ~/wallbox-project

# Verify
ls -la
```

---

## Building the Project

### 1. Prepare Build Environment

```bash
cd ~/wallbox-project

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Clean any previous builds
rm -rf CMakeCache.txt CMakeFiles/
```

### 2. Configure CMake

```bash
# Generate build files
cmake ..

# Or with specific options:
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DENABLE_GPIO=ON

# For debug build:
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

**Expected Output**:

```
-- The C compiler identification is GNU 11.4.0
-- The CXX compiler identification is GNU 11.4.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/pi/wallbox-project/build
```

### 3. Build the Project

```bash
# Build all targets
make

# Or build specific targets:
make wallbox_control_v3
make simulator

# Build with multiple cores (faster)
make -j$(nproc)

# Verbose build (shows all commands)
make VERBOSE=1
```

**Expected Output**:

```
Scanning dependencies of target wallbox_control_v3
[ 11%] Building CXX object CMakeFiles/wallbox_control_v3.dir/src/main_v3.cpp.o
[ 22%] Building CXX object CMakeFiles/wallbox_control_v3.dir/src/WallboxController.cpp.o
[ 33%] Building CXX object CMakeFiles/wallbox_control_v3.dir/src/ChargingStateMachine.cpp.o
[ 44%] Building CXX object CMakeFiles/wallbox_control_v3.dir/src/BananaPiGpioController.cpp.o
[ 55%] Building CXX object CMakeFiles/wallbox_control_v3.dir/src/UdpCommunicator.cpp.o
[ 66%] Building CXX object CMakeFiles/wallbox_control_v3.dir/src/HttpApiServer.cpp.o
[ 77%] Building CXX object CMakeFiles/wallbox_control_v3.dir/src/IsoStackCtrlProtocol_impl.cpp.o
[ 88%] Linking CXX executable wallbox_control_v3
[100%] Built target wallbox_control_v3
```

### 4. Verify Build

```bash
# Check if binary was created
ls -lh wallbox_control_v3

# Check binary type
file wallbox_control_v3
# Should show: ELF 32-bit LSB executable, ARM

# Test help output
./wallbox_control_v3 --help

# Check dependencies
ldd wallbox_control_v3
```

### 5. Build Troubleshooting

#### Missing libmicrohttpd

```bash
# Error: "fatal error: microhttpd.h: No such file or directory"
sudo apt install libmicrohttpd-dev -y
```

#### CMake Version Too Old

```bash
# Error: "CMake 3.10 or higher is required"
# Install newer CMake from Kitware
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt update
sudo apt install cmake -y
```

#### GPIO Library Not Found

```bash
# For Banana Pi, install WiringPi from source
git clone https://github.com/BPI-SINOVOIP/BPI-WiringPi2.git
cd BPI-WiringPi2
./build
```

---

## Configuration

### 1. Create Configuration File

```bash
cd ~/wallbox-project/build

# Create config.json
nano config.json
```

**Basic Configuration**:

```json
{
  "mode": "development",
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "<SIM_HOST>",
    "api_port": 8080
  },
  "gpio_pins": {
    "relay_enable": 21,
    "led_green": 17,
    "led_yellow": 27,
    "led_red": 22,
    "button": 23
  },
  "charging": {
    "max_current_amps": 16,
    "voltage": 230,
    "timeout_seconds": 300
  },
  "logging": {
    "level": "info",
    "file": "/tmp/wallbox_v3.log",
    "simulator_file": "/tmp/wallbox_simulator.log"
  }
}
```

**Production Configuration**:

```json
{
  "mode": "production",
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "<SIM_HOST>",
    "api_port": 8080
  },
  "gpio_pins": {
    "relay_enable": 4,
    "led_green": 17,
    "led_yellow": 27,
    "led_red": 22,
    "button": 23
  },
  "charging": {
    "max_current_amps": 32,
    "voltage": 230,
    "timeout_seconds": 600
  },
  "logging": {
    "level": "warning",
    "file": "/var/log/wallbox/wallbox.log",
    "simulator_file": "/var/log/wallbox/simulator.log"
  },
  "security": {
    "enable_authentication": true,
    "api_key_required": true
  }
}
```

### 2. GPIO Pin Mapping

#### Raspberry Pi GPIO

```
Physical Pin | BCM Pin | WiringPi | Usage
-------------|---------|----------|------------------
Pin 7        | GPIO 4  | 7        | Relay Enable
Pin 11       | GPIO 17 | 0        | LED Green
Pin 13       | GPIO 27 | 2        | LED Yellow
Pin 15       | GPIO 22 | 3        | LED Red
Pin 16       | GPIO 23 | 4        | Button Input
```

#### Banana Pi GPIO

```
Physical Pin | Pin Name | Usage
-------------|----------|------------------
Pin 7        | GPIO 4   | Relay Enable
Pin 11       | GPIO 17  | LED Green
Pin 13       | GPIO 27  | LED Yellow
Pin 15       | GPIO 22  | LED Red
Pin 16       | GPIO 23  | Button Input
```

### 3. Network Configuration

```bash
# Edit config.json network section
nano config.json

# Set correct IP addresses:
# - udp_send_address: IP of your simulator/development machine
# - api_port: HTTP API port (default 8080)
# - udp_listen_port: Port for receiving UDP messages (default 50010)
# - udp_send_port: Port for sending UDP messages (default 50011)
```

### 4. Validate Configuration

```bash
# Test JSON syntax
python3 -m json.tool config.json

# Or use jq
jq . config.json

# Check file permissions
ls -l config.json
# Should be readable: -rw-r--r--
```

---

## Running the Wallbox

### 1. Test Run (Foreground)

```bash
cd ~/wallbox-project/build

# Run in API mode (default)
./wallbox_control_v3

# Run in interactive mode
./wallbox_control_v3 --interactive

# Run in dual mode (API + Interactive)
./wallbox_control_v3 --dual
```

**Expected Startup Output**:

```
==================================================
  Wallbox Controller v3.0 - With REST API
==================================================
Loading configuration from config.json...

╔════════════════════════════════════════════════╗
║  MODE: DEVELOPMENT 🔧                          ║
╚════════════════════════════════════════════════╝

Configuration:
  Mode: development
  GPIO Type: stub (simulator)
  UDP Listen Port: 50010
  UDP Send Port: 50011
  UDP Send Address: <SIM_HOST>
  REST API Port: 8080

[GPIO Factory] Creating stub GPIO controller (development mode)
Initializing Wallbox Controller...
UDP communicator connected on port 50010
Wallbox Controller initialized successfully
Starting HTTP API server...
HTTP server started on port 8080

Wallbox Controller is running. Press Ctrl+C to stop.
```

### 2. Run in Background

```bash
# Start in background
./wallbox_control_v3 </dev/null >/tmp/wallbox.log 2>&1 &

# Save PID
echo $! > /tmp/wallbox.pid

# Verify running
ps aux | grep wallbox_control | grep -v grep

# Check logs
tail -f /tmp/wallbox.log
```

### 3. Using Screen (Recommended for Development)

```bash
# Install screen
sudo apt install screen -y

# Start screen session
screen -S wallbox

# Run wallbox
cd ~/wallbox-project/build
./wallbox_control_v3

# Detach from screen: Ctrl+A, then D

# Reattach to screen
screen -r wallbox

# List screens
screen -ls

# Kill screen session
screen -X -S wallbox quit
```

### 4. Using Systemd Service (Production)

Create service file:

```bash
sudo nano /etc/systemd/system/wallbox.service
```

**Service File Content**:

```ini
[Unit]
Description=Wallbox Controller Service
After=network.target
Wants=network-online.target

[Service]
Type=simple
User=pi
Group=pi
WorkingDirectory=/home/pi/wallbox-project/build
ExecStart=/home/pi/wallbox-project/build/wallbox_control_v3
Restart=always
RestartSec=10
StandardOutput=journal
StandardError=journal

# Security settings
NoNewPrivileges=true
PrivateTmp=true
ProtectSystem=strict
ProtectHome=read-only
ReadWritePaths=/tmp /var/log/wallbox

# Resource limits
LimitNOFILE=4096
MemoryLimit=100M

[Install]
WantedBy=multi-user.target
```

**Enable and Start Service**:

```bash
# Reload systemd
sudo systemctl daemon-reload

# Enable service (start on boot)
sudo systemctl enable wallbox

# Start service
sudo systemctl start wallbox

# Check status
sudo systemctl status wallbox

# View logs
sudo journalctl -u wallbox -f

# Stop service
sudo systemctl stop wallbox

# Restart service
sudo systemctl restart wallbox
```

---

## Testing

### 1. Test HTTP API

```bash
# From Pi itself
curl http://localhost:8080/api/status

# From development machine
curl http://<API_HOST>:8080/api/status

# Expected response:
# {"state":"IDLE","wallboxEnabled":true,"relayEnabled":false,"charging":false,"timestamp":1234567890}
```

**All API Endpoints**:

```bash
# Health check
curl http://<API_HOST>:8080/health

# Get status
curl http://<API_HOST>:8080/api/status

# Get relay status
curl http://<API_HOST>:8080/api/relay

# Enable wallbox
curl -X POST http://<API_HOST>:8080/api/wallbox/enable

# Disable wallbox
curl -X POST http://<API_HOST>:8080/api/wallbox/disable

# Start charging
curl -X POST http://<API_HOST>:8080/api/charging/start

# Stop charging
curl -X POST http://<API_HOST>:8080/api/charging/stop

# Pause charging
curl -X POST http://<API_HOST>:8080/api/charging/pause

# Resume charging
curl -X POST http://<API_HOST>:8080/api/charging/resume
```

### 2. Test UDP Communication

**From Development Machine**:

```bash
# Build and run simulator
cd /path/to/project/build/bin
make simulator
./simulator

# Configure UDP
> setudp <API_HOST> 50011 50010

# Test commands
> on
> status
> charge
> status
> off
> quit
```

### 3. Test Interactive Mode

```bash
cd ~/wallbox-project/build
./wallbox_control_v3 --interactive

# Try commands:
> help
> status
> enable
> on
> charge
> status
> off
> disable
> quit
```

### 4. Load Testing

```bash
# Install Apache Bench
sudo apt install apache2-utils -y

# Test API performance
ab -n 1000 -c 10 http://<API_HOST>:8080/api/status

# Results show:
# - Requests per second
# - Time per request
# - Transfer rate
```

### 5. Monitor Resource Usage

```bash
# Real-time monitoring
htop

# Check wallbox process
top -p $(pgrep wallbox_control)

# Memory usage
ps aux | grep wallbox_control | awk '{print $4"%", $6/1024"MB"}'

# Network connections
lsof -i -P -n | grep wallbox

# Check ports
ss -tulpn | grep -E "8080|50010|50011"
```

---

## Production Deployment

### 1. Create Log Directory

```bash
# Create log directory
sudo mkdir -p /var/log/wallbox

# Set permissions
sudo chown -R $USER:$USER /var/log/wallbox

# Create log rotation config
sudo nano /etc/logrotate.d/wallbox
```

**Log Rotation Config**:

```
/var/log/wallbox/*.log {
    daily
    rotate 7
    compress
    delaycompress
    missingok
    notifempty
    create 644 pi pi
    sharedscripts
    postrotate
        systemctl reload wallbox >/dev/null 2>&1 || true
    endscript
}
```

### 2. Security Hardening

```bash
# Create dedicated user
sudo useradd -r -s /bin/false wallbox

# Set ownership
sudo chown -R wallbox:wallbox /home/pi/wallbox-project

# Update systemd service to use wallbox user
sudo nano /etc/systemd/system/wallbox.service
# Change: User=wallbox, Group=wallbox

# Reload and restart
sudo systemctl daemon-reload
sudo systemctl restart wallbox
```

### 3. Enable Automatic Updates

```bash
# Install unattended-upgrades
sudo apt install unattended-upgrades -y

# Configure
sudo dpkg-reconfigure -plow unattended-upgrades

# Edit configuration
sudo nano /etc/apt/apt.conf.d/50unattended-upgrades
```

### 4. Setup Monitoring

**Create Health Check Script**:

```bash
nano ~/wallbox-project/scripts/health-check.sh
```

```bash
#!/bin/bash
# Wallbox Health Check Script

API_URL="http://localhost:8080/api/status"
LOG_FILE="/var/log/wallbox/health-check.log"
TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')

# Check if process is running
if ! pgrep -x wallbox_control > /dev/null; then
    echo "[$TIMESTAMP] ERROR: Wallbox process not running" >> "$LOG_FILE"
    sudo systemctl start wallbox
    exit 1
fi

# Check API response
HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" "$API_URL")
if [ "$HTTP_CODE" != "200" ]; then
    echo "[$TIMESTAMP] ERROR: API returned HTTP $HTTP_CODE" >> "$LOG_FILE"
    sudo systemctl restart wallbox
    exit 1
fi

echo "[$TIMESTAMP] OK: Wallbox healthy" >> "$LOG_FILE"
exit 0
```

```bash
# Make executable
chmod +x ~/wallbox-project/scripts/health-check.sh

# Add to crontab (run every 5 minutes)
crontab -e

# Add line:
*/5 * * * * /home/pi/wallbox-project/scripts/health-check.sh
```

### 5. Backup Configuration

```bash
# Create backup script
nano ~/wallbox-project/scripts/backup-config.sh
```

```bash
#!/bin/bash
# Backup wallbox configuration

BACKUP_DIR="/home/pi/wallbox-backups"
TIMESTAMP=$(date '+%Y%m%d_%H%M%S')
PROJECT_DIR="/home/pi/wallbox-project"

# Create backup directory
mkdir -p "$BACKUP_DIR"

# Create backup
tar -czf "$BACKUP_DIR/wallbox-config-$TIMESTAMP.tar.gz" \
    -C "$PROJECT_DIR" \
    build/config.json \
    build/*.json \
    2>/dev/null

# Keep only last 10 backups
cd "$BACKUP_DIR"
ls -t | tail -n +11 | xargs -r rm

echo "Backup created: wallbox-config-$TIMESTAMP.tar.gz"
```

```bash
# Make executable
chmod +x ~/wallbox-project/scripts/backup-config.sh

# Run daily at 2 AM
crontab -e
# Add:
0 2 * * * /home/pi/wallbox-project/scripts/backup-config.sh
```

### 6. Setup Remote Access (Optional)

```bash
# Install and configure VPN (WireGuard)
sudo apt install wireguard -y

# Or use SSH tunneling
ssh -L 8080:localhost:8080 pi@<API_HOST>

# Now access from local machine:
curl http://localhost:8080/api/status
```

---

## Troubleshooting

### Common Issues

#### 1. Build Fails - Missing Headers

**Error**: `fatal error: microhttpd.h: No such file or directory`

**Solution**:

```bash
sudo apt update
sudo apt install libmicrohttpd-dev -y
cd ~/wallbox-project/build
cmake ..
make clean
make
```

#### 2. Permission Denied on GPIO

**Error**: `GPIO initialization failed: Permission denied`

**Solution**:

```bash
# Add user to gpio group
sudo usermod -a -G gpio $USER

# Or run with sudo (not recommended for production)
sudo ./wallbox_control_v3

# Logout and login for group changes to take effect
```

#### 3. Port Already in Use

**Error**: `Failed to bind HTTP server to port 8080`

**Solution**:

```bash
# Find what's using the port
sudo lsof -i :8080

# Kill the process
sudo kill -9 <PID>

# Or use a different port in config.json
nano config.json
# Change "api_port": 8081
```

#### 4. UDP Communication Not Working

**Check 1 - Firewall**:

```bash
# Check firewall status
sudo ufw status

# Allow UDP ports
sudo ufw allow 50010/udp
sudo ufw allow 50011/udp
```

**Check 2 - Port Configuration**:

```bash
# Verify config.json has correct settings
cat config.json | grep -A5 network

# Wallbox should have:
# "udp_listen_port": 50010
# "udp_send_port": 50011
# "udp_send_address": "<simulator_IP>"
```

**Check 3 - Network Connectivity**:

```bash
# Ping simulator machine
ping <SIM_HOST>

# Test UDP port is open
nc -u -z -v <API_HOST> 50010
```

#### 5. Wallbox Crashes on Startup

**Check Logs**:

```bash
# Check system logs
sudo journalctl -u wallbox -n 50

# Check application logs
tail -50 /tmp/wallbox.log

# Run in foreground to see errors
cd ~/wallbox-project/build
./wallbox_control_v3
```

**Common Causes**:

- Invalid config.json syntax
- Missing libraries
- GPIO permission issues
- Port conflicts

#### 6. High CPU Usage

```bash
# Check process stats
top -p $(pgrep wallbox_control)

# Enable debug logging temporarily
# Edit config.json: "level": "debug"
sudo systemctl restart wallbox

# Check for infinite loops in logs
tail -f /var/log/wallbox/wallbox.log
```

#### 7. Service Won't Start on Boot

```bash
# Check service status
sudo systemctl status wallbox

# Check if enabled
sudo systemctl is-enabled wallbox

# Enable if not
sudo systemctl enable wallbox

# Check service file
sudo systemctl cat wallbox

# Verify paths exist
ls -l /home/pi/wallbox-project/build/wallbox_control_v3
```

### Diagnostic Commands

```bash
# Check system info
uname -a
cat /proc/cpuinfo | grep Hardware

# Check GPIO
gpio readall  # For WiringPi

# Check network
ifconfig
netstat -tulpn | grep wallbox

# Check memory
free -h
cat /proc/meminfo

# Check disk space
df -h

# Check running processes
ps aux | grep wallbox

# Check open files
lsof -p $(pgrep wallbox_control)

# Check system load
uptime
```

### Debug Mode

```bash
# Run with debug output
cd ~/wallbox-project/build

# Set environment variable
export WALLBOX_DEBUG=1

# Run
./wallbox_control_v3

# Or use GDB
gdb ./wallbox_control_v3
(gdb) run
```

### Getting Help

1. **Check Documentation**:

   - README.md
   - API_REFERENCE.md
   - Architecture documentation in docs/

2. **Check Logs**:

   - `/tmp/wallbox.log` or `/var/log/wallbox/wallbox.log`
   - System logs: `sudo journalctl -u wallbox`

3. **Community Support**:

   - GitHub Issues
   - Project Wiki
   - Developer Forum

4. **Report Bugs**:
   - Include: OS version, hardware model, error logs
   - Steps to reproduce
   - Expected vs actual behavior

---

## Quick Reference

### Essential Commands

```bash
# Build
cd ~/wallbox-project/build && make

# Run (foreground)
./wallbox_control_v3

# Run (background)
./wallbox_control_v3 </dev/null >/tmp/wallbox.log 2>&1 &

# Service control
sudo systemctl start wallbox
sudo systemctl stop wallbox
sudo systemctl restart wallbox
sudo systemctl status wallbox

# View logs
tail -f /tmp/wallbox.log
sudo journalctl -u wallbox -f

# Test API
curl http://localhost:8080/api/status

# Check ports
sudo lsof -i :8080
sudo lsof -i :50010

# Kill process
pkill -9 wallbox_control
```

### File Locations

```
Project:       ~/wallbox-project/
Binary:        ~/wallbox-project/build/wallbox_control_v3
Config:        ~/wallbox-project/build/config.json
Logs:          /tmp/wallbox.log or /var/log/wallbox/
Service:       /etc/systemd/system/wallbox.service
Backups:       ~/wallbox-backups/
Scripts:       ~/wallbox-project/scripts/
```

### Network Ports

```
HTTP API:      8080/tcp
UDP Receive:   50010/udp
UDP Send:      50011/udp
SSH:           22/tcp
```

---

## Next Steps

After successful installation:

1. ✅ **Configure production settings** in config.json
2. ✅ **Setup systemd service** for automatic startup
3. ✅ **Configure log rotation**
4. ✅ **Setup monitoring and health checks**
5. ✅ **Configure firewall rules**
6. ✅ **Setup automated backups**
7. ✅ **Test all API endpoints**
8. ✅ **Test UDP communication**
9. ✅ **Document your specific GPIO wiring**
10. ✅ **Create emergency shutdown procedure**

## Related Documentation

- [API Reference](./api/API_REFERENCE.md)
- [Architecture Overview](./architecture/ARCHITECTURE_V3.md)
- [Development Guide](./guides/DEVELOPMENT.md)
- [UDP Sync Guide](./UDP_SYNC_GUIDE.md)
- [Dual Mode Guide](./DUAL_MODE_GUIDE.md)
- [Build and Run Session](./BUILD_AND_RUN_SESSION.md)

---

## Appendix

### A. GPIO Pin Reference Chart

Download and print GPIO pin diagrams for your specific board:

- Raspberry Pi: https://pinout.xyz/
- Banana Pi: http://wiki.banana-pi.org/Getting_Started_with_BPI-M2

### B. Troubleshooting Decision Tree

```
Problem: Wallbox won't start
│
├─> Check: Process running?
│   ├─> No: Start service
│   └─> Yes: Check logs
│
├─> Check: Port available?
│   ├─> No: Kill conflicting process
│   └─> Yes: Check config
│
├─> Check: Config valid?
│   ├─> No: Fix JSON syntax
│   └─> Yes: Check permissions
│
└─> Check: GPIO permissions?
    ├─> No: Add to gpio group
    └─> Yes: Check hardware
```

### C. Performance Tuning

For optimal performance on resource-constrained devices:

```json
{
  "performance": {
    "worker_threads": 2,
    "max_connections": 10,
    "buffer_size": 4096,
    "timeout_ms": 5000
  }
}
```

### D. Security Checklist

- [ ] Changed default passwords
- [ ] Firewall configured and enabled
- [ ] SSH key-based authentication only
- [ ] Dedicated wallbox user created
- [ ] Log rotation configured
- [ ] Regular backups scheduled
- [ ] Health monitoring active
- [ ] Services run as non-root user
- [ ] File permissions properly set
- [ ] Network access restricted

---

**Document Version**: 1.0  
**Last Updated**: December 12, 2025  
**Tested On**: Raspberry Pi 4B, Banana Pi M2+  
**OS Versions**: Raspberry Pi OS Bookworm, Armbian Bookworm  
**Support**: See project README for contact information
