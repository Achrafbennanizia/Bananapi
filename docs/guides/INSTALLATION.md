# Installation Guide (LEGACY)

> **⚠️ LEGACY DOCUMENT - FOR HISTORICAL REFERENCE ONLY**
>
> **This document is outdated and retained only for historical purposes.**
>
> **👉 Please use the [Complete Installation Guide](INSTALLATION_GUIDE.md) instead.**
>
> The canonical installation guide contains up-to-date instructions for the current project structure.

---

## Table of Contents

- [System Requirements](#system-requirements)
- [Installation Methods](#installation-methods)
- [Platform-Specific Instructions](#platform-specific-instructions)
- [Verification](#verification)
- [Post-Installation](#post-installation)

## System Requirements

### Minimum Requirements

- **CPU:** 1 GHz single-core processor
- **RAM:** 512 MB
- **Storage:** 100 MB free space
- **OS:** Linux, macOS, or Windows (WSL2)
- **Compiler:** C++17 compatible (GCC 7+, Clang 5+)

### Recommended Requirements

- **CPU:** 1.5 GHz dual-core processor
- **RAM:** 1 GB
- **Storage:** 500 MB free space
- **Network:** Ethernet or WiFi for remote deployment
- **GPIO:** Raspberry Pi or Banana Pi for hardware control

## Installation Methods

### Method 1: Quick Install (Recommended)

```bash
# Clone repository
git clone https://github.com/Achrafbennanizia/Bananapi.git
cd Bananapi/WallboxCtrl/src

# Compile
g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl
g++ -std=c++17 -O2 simulator.cpp IsoStackCtrlProtocol_impl.cpp -o simulator

# Run
./wallbox_ctrl  # Terminal 1
./simulator     # Terminal 2
```

### Method 2: CMake Build

```bash
cd Bananapi/WallboxCtrl
mkdir -p build && cd build

# Configure
cmake ..

# Build
make -j$(nproc)

# Install (optional)
sudo make install

# Run
./wallbox_control
```

### Method 3: Docker Installation

```bash
cd Bananapi/env

# Build container
docker-compose build

# Run
docker-compose up
```

### Method 4: Build Script

```bash
cd Bananapi/WallboxCtrl

# Make executable
chmod +x build.sh

# Build
./build.sh

# Executables will be in src/
cd src
./wallbox_ctrl
./simulator
```

## Platform-Specific Instructions

### Ubuntu / Debian

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential git cmake

# Install GPIO support (Raspberry Pi)
sudo apt-get install -y wiringpi

# Clone and build
git clone https://github.com/Achrafbennanizia/Bananapi.git
cd Bananapi/WallboxCtrl/src
g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl
g++ -std=c++17 -O2 simulator.cpp IsoStackCtrlProtocol_impl.cpp -o simulator

# Run with GPIO (requires root)
sudo ./wallbox_ctrl
```

### Raspberry Pi OS

```bash
# Update system
sudo apt-get update && sudo apt-get upgrade -y

# Install dependencies
sudo apt-get install -y git g++ cmake wiringpi

# Clone repository
git clone https://github.com/Achrafbennanizia/Bananapi.git
cd Bananapi/WallboxCtrl/src

# Build
g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl
g++ -std=c++17 -O2 simulator.cpp IsoStackCtrlProtocol_impl.cpp -o simulator

# Configure GPIO permissions (optional, avoids sudo)
sudo usermod -a -G gpio $USER
sudo reboot

# Run
./wallbox_ctrl
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install git cmake

# Clone and build
git clone https://github.com/Achrafbennanizia/Bananapi.git
cd Bananapi/WallboxCtrl/src
g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl
g++ -std=c++17 -O2 simulator.cpp IsoStackCtrlProtocol_impl.cpp -o simulator

# Run
./wallbox_ctrl
```

### Windows (WSL2)

```bash
# Install WSL2 (PowerShell as Administrator)
wsl --install -d Ubuntu

# Open Ubuntu terminal
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential git cmake

# Clone and build
git clone https://github.com/Achrafbennanizia/Bananapi.git
cd Bananapi/WallboxCtrl/src
g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl
g++ -std=c++17 -O2 simulator.cpp IsoStackCtrlProtocol_impl.cpp -o simulator

# Run
./wallbox_ctrl
```

### Banana Pi

```bash
# Update system
sudo apt-get update && sudo apt-get upgrade -y

# Install dependencies
sudo apt-get install -y git g++ cmake

# Clone repository
git clone https://github.com/Achrafbennanizia/Bananapi.git
cd Bananapi/WallboxCtrl/src

# Build
g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl
g++ -std=c++17 -O2 simulator.cpp IsoStackCtrlProtocol_impl.cpp -o simulator

# Run with GPIO permissions
sudo ./wallbox_ctrl
```

## Verification

### Check Compilation

```bash
# Verify executables exist
ls -lh wallbox_ctrl simulator

# Expected output:
# -rwxr-xr-x  1 user  staff   40K  wallbox_ctrl
# -rwxr-xr-x  1 user  staff   57K  simulator
```

### Test Run

```bash
# Terminal 1
./wallbox_ctrl

# Expected output:
# [HAL] GPIO system initialized (stub)
# [HAL] pinMode(5, OUTPUT)
# Wallbox Controller starting...
# Listening on: *:50010
# Sending to: 127.0.0.1:50011
# ...

# Terminal 2
./simulator

# Expected output:
# ISO 15118 Stack Simulator starting...
# Sending to: 127.0.0.1:50010
# Listening on: *:50011
# ...
```

### Network Test

```bash
# Check if ports are listening
sudo lsof -i :50010
sudo lsof -i :50011

# Or using netstat
netstat -an | grep 50010
netstat -an | grep 50011
```

### Functionality Test

In wallbox_ctrl terminal:

```
> enable
> status
```

In simulator terminal:

```
> on
> ready
> status
```

You should see communication between both programs.

## Post-Installation

### Create Systemd Service (Linux)

```bash
# Create service file
sudo nano /etc/systemd/system/wallbox_ctrl.service
```

```ini
[Unit]
Description=Wallbox Controller
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/home/pi/Bananapi/WallboxCtrl/src
ExecStart=/home/pi/Bananapi/WallboxCtrl/src/wallbox_ctrl
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

```bash
# Enable and start service
sudo systemctl daemon-reload
sudo systemctl enable wallbox_ctrl
sudo systemctl start wallbox_ctrl

# Check status
sudo systemctl status wallbox_ctrl
```

### Configure Firewall

```bash
# UFW (Ubuntu/Debian)
sudo ufw allow 50010/udp
sudo ufw allow 50011/udp

# firewalld (RHEL/CentOS)
sudo firewall-cmd --permanent --add-port=50010/udp
sudo firewall-cmd --permanent --add-port=50011/udp
sudo firewall-cmd --reload

# iptables
sudo iptables -A INPUT -p udp --dport 50010 -j ACCEPT
sudo iptables -A INPUT -p udp --dport 50011 -j ACCEPT
sudo iptables-save > /etc/iptables/rules.v4
```

### Setup Auto-Start (Raspberry Pi)

```bash
# Edit rc.local
sudo nano /etc/rc.local

# Add before 'exit 0':
/home/pi/Bananapi/WallboxCtrl/src/wallbox_ctrl &

# Make rc.local executable
sudo chmod +x /etc/rc.local
```

### Create Desktop Shortcut (Linux Desktop)

```bash
# Create desktop file
nano ~/.local/share/applications/wallbox_ctrl.desktop
```

```ini
[Desktop Entry]
Name=Wallbox Controller
Comment=ISO 15118 Wallbox Control
Exec=/home/user/Bananapi/WallboxCtrl/src/wallbox_ctrl
Icon=utilities-terminal
Terminal=true
Type=Application
Categories=Utility;
```

## Troubleshooting Installation

### Compilation Errors

**Error:** `g++: command not found`

```bash
# Install compiler
sudo apt-get install build-essential  # Ubuntu/Debian
brew install gcc                        # macOS
```

**Error:** `fatal error: arpa/inet.h: No such file or directory`

```bash
# Install development headers
sudo apt-get install libc6-dev
```

**Error:** `C++17 not supported`

```bash
# Check compiler version
g++ --version

# Update compiler if needed
sudo apt-get install g++-9  # Ubuntu
brew upgrade gcc            # macOS
```

### Runtime Errors

**Error:** `bind: Permission denied`

```bash
# Use ports > 1024 or run with sudo
sudo ./wallbox_ctrl
```

**Error:** `bind: Address already in use`

```bash
# Kill process using the port
lsof -ti:50010 | xargs kill -9
```

### GPIO Errors

**Error:** `Unable to open GPIO` (Raspberry Pi)

```bash
# Run with sudo
sudo ./wallbox_ctrl

# Or configure permissions
sudo usermod -a -G gpio $USER
```

## Next Steps

After successful installation:

1. Read the [User Guide](USAGE.md)
2. Review the [API Documentation](README.md#api-documentation)
3. Try the [Interactive Guide](WallboxCtrl/INTERACTIVE_GUIDE.md)
4. Explore [Development Guide](DEVELOPMENT.md)

## Support

If you encounter issues:

- Check [Troubleshooting](README.md#troubleshooting)
- Review [GitHub Issues](https://github.com/Achrafbennanizia/Bananapi/issues)
- Create a new issue with:
  - OS and version
  - Compiler version (`g++ --version`)
  - Full error message
  - Steps to reproduce
