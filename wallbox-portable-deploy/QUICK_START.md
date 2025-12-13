# Wallbox Portable Deploy - Quick Start Guide

**Version:** 4.1 (with CP Signal System)  
**Date:** December 13, 2025

This directory contains a C++14 compatible, portable version of the Wallbox Controller with **CP Signal System** ready for deployment to any Raspberry Pi, Banana Pi, or similar ARM SBC.

## 📦 What's in this package?

- ✅ All source code (47 files, ~9,400 LOC)
- ✅ **CP Signal System** (hardware + simulator modes)
- ✅ ISO 15118 compliant state machine
- ✅ C++14 compatible CMakeLists.txt
- ✅ Default configuration (config/config.json)
- ✅ Enhanced installation script (scripts/install.sh)
- ✅ Smart deployment script (scripts/deploy.sh)
- ✅ Automated test suite (test_portable.sh)
- ✅ Comprehensive documentation

## 🚀 Quick Deployment

### Method 1: One-Command Deployment (Recommended)

**Production mode (hardware GPIO):**

```bash
# From this directory on your Mac/PC
./scripts/deploy.sh 192.168.178.34

# With custom user
PI_USER=root ./scripts/deploy.sh 192.168.178.34
```

**Development mode (simulator):**

```bash
# Use simulator instead of hardware
BUILD_MODE=development ./scripts/deploy.sh 192.168.178.34
```

**What the script does:**

1. ✅ Packages the entire project (includes CP Signal System)
2. ✅ Copies to Pi via SSH
3. ✅ Installs dependencies (libmicrohttpd, libcurl)
4. ✅ Builds with C++14 using CMake
5. ✅ Configures for production/development mode
6. ✅ Sets up GPIO permissions
7. ✅ Creates systemd service
8. ✅ Starts the wallbox controller
9. ✅ Tests API and CP signal system
10. ✅ Provides status summary

### Method 2: Build with Make (Alternative)

```bash
# Copy to Pi
scp -r ../wallbox-portable-deploy pi@192.168.178.34:~/

# SSH and build
ssh pi@192.168.178.34
cd wallbox-portable-deploy
make              # Build everything
make install      # Install to system (optional)
cd build && ./wallbox_control_v3
```

### Method 3: Manual Installation

```bash
# Copy this entire folder to your Pi
scp -r ../wallbox-portable-deploy pi@192.168.178.34:~/

# SSH into Pi
ssh pi@192.168.178.34

# Run installer
cd wallbox-portable-deploy
BUILD_MODE=production ./scripts/install.sh
```

## 🧪 Testing

### Automated Test Suite

```bash
# Run comprehensive tests (11 test cases)
cd wallbox-portable-deploy
./test_portable.sh
```

**Tests include:**

- ✅ Process verification
- ✅ Network port binding
- ✅ API health check
- ✅ Full charging cycle (4 state transitions)
- ✅ Wallbox enable/disable
- ✅ Resource usage monitoring
- ✅ Log file generation

### Test After Deployment

```bash
# From your computer
curl http://192.168.178.34:8080/api/status

# Expected response:
{
    "state": "IDLE",
    "wallboxEnabled": true,
    "relayEnabled": false,
    "charging": false,
    "timestamp": 1734123456
}
```

### Manual Tests

```bash
# Health check
curl http://192.168.178.34:8080/health

# Start charging
curl -X POST http://192.168.178.34:8080/api/charging/start

# Check relay status
curl http://192.168.178.34:8080/api/relay

# View logs
ssh pi@192.168.178.34 'tail -f /tmp/wallbox.log'

# Check service status
ssh pi@192.168.178.34 'sudo systemctl status wallbox'
```

## 📋 Features

All features from the main wallbox project v4.1:

### Core Features

- ✅ **ISO 15118 Compliance**: 9 charging states (OFF, IDLE, CONNECTED, IDENTIFICATION, READY, CHARGING, STOP, FINISHED, ERROR)
- ✅ **CP Signal System**: Read Control Pilot signals (hardware + simulator)
- ✅ **Dual Mode Operation**: Production (hardware) or Development (simulator)
- ✅ **HTTP REST API**: Full RESTful API on port 8080
- ✅ **UDP Communication**: ISO 15118 protocol (ports 50010/50011)
- ✅ **State Machine**: Robust charging state management
- ✅ **GPIO Control**: Direct hardware control (BananaPi/RaspberryPi)
- ✅ **Auto-restart**: Systemd service with watchdog
- ✅ **C++14 Compatible**: Works on all Pi platforms

### CP Signal System (NEW in v4.1)

- ✅ **Hardware Mode**: Reads CP voltage from GPIO pin 7 (ADC)
- ✅ **Simulator Mode**: Receives CP states via UDP
- ✅ **IEC 61851-1 Compliant**: Voltage thresholds (12V, 9V, 6V, 3V, 0V, -12V)
- ✅ **Real-time Monitoring**: 100ms polling interval
- ✅ **State Callbacks**: Event-driven architecture
- ✅ **Factory Pattern**: Mode-based reader creation

### Design Patterns

- ✅ **Strategy Pattern**: Interchangeable CP readers
- ✅ **Factory Pattern**: Object creation abstraction
- ✅ **Observer Pattern**: State change notifications
- ✅ **Singleton Pattern**: Global configuration
- ✅ **Dependency Injection**: Loose coupling

### SOLID Principles

- ✅ Single Responsibility
- ✅ Open/Closed
- ✅ Liskov Substitution
- ✅ Interface Segregation
- ✅ Dependency Inversion

## 🔧 Key Improvements in v4.1

1. **CP Signal System**: Complete implementation with hardware + simulator support
2. **ISO 15118 States**: Full compliance with 9 charging states
3. **Enhanced Scripts**: Smart deployment with mode detection
4. **Better Testing**: Comprehensive 11-test suite
5. **GPIO Permissions**: Automatic setup for production mode
6. **Python Automation**: JSON config manipulation
7. **Architecture Detection**: Auto-detect platform capabilities
8. **Installation Verification**: Complete validation checks

## 📝 Configuration

Default config is in `config/config.json`:

```json
{
  "mode": "production",
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "192.168.178.23",
    "api_port": 8080
  },
  "gpio_pins": {
    "relay_enable": 21,
    "led_green": 17,
    "led_yellow": 27,
    "led_red": 22,
    "button": 23,
    "cp_pin": 7
  },
  "charging": {
    "max_current_amps": 16,
    "voltage": 230,
    "timeout_seconds": 300
  }
}
```

**Modes:**

- `production`: Hardware GPIO, CP from pin 7, requires permissions
- `development`: Simulated GPIO, CP via UDP, no hardware needed

## 🎯 Platform Support

**Tested and verified on:**

- ✅ Raspberry Pi (all models with GPIO)
- ✅ Banana Pi M1, M2, M64
- ✅ Orange Pi One, PC, Plus
- ✅ Any Debian/Ubuntu ARM SBC with GPIO

**Architecture support:**

- ✅ ARMv7l (32-bit)
- ✅ ARMv6l (32-bit)
- ✅ AArch64/ARM64 (64-bit)
- ⚠️ x86_64 (development mode only)

## 📚 Documentation

Comprehensive documentation available:

- **README.md** - Complete system documentation
- **QUICK_START.md** - This guide
- **BUILD_METHODS.md** - Build system details
- **PORTABLE_ARCHITECTURE.md** - Architecture overview
- **PORTABLE_TEST_RESULTS.md** - Test results and metrics
- **PACKAGE_CONTENTS.md** - File inventory
- **docs/** - Additional guides and references

## ⚡ Quick Commands Reference

```bash
# Deploy to Pi (production mode)
./scripts/deploy.sh 192.168.178.34

# Deploy in development mode
BUILD_MODE=development ./scripts/deploy.sh 192.168.178.34

# Deploy as root user
PI_USER=root ./scripts/deploy.sh 192.168.178.34

# Run automated tests
./test_portable.sh

# Check API status
curl http://192.168.178.34:8080/api/status

# Start charging
curl -X POST http://192.168.178.34:8080/api/charging/start

# Stop charging
curl -X POST http://192.168.178.34:8080/api/charging/stop

# View logs (journalctl)
ssh pi@192.168.178.34 'sudo journalctl -u wallbox -f'

# View logs (file)
ssh pi@192.168.178.34 'tail -f /tmp/wallbox.log'

# Restart service
ssh pi@192.168.178.34 'sudo systemctl restart wallbox'

# Check process
ssh pi@192.168.178.34 'ps aux | grep wallbox_control'

# Check ports
ssh pi@192.168.178.34 'lsof -i :8080 -i :50010 -i :50011'
```

## ✅ Pre-deployment Checklist

**Hardware:**

- [ ] Pi has network connection (wired or WiFi)
- [ ] Pi is powered and booted
- [ ] GPIO pins accessible (production mode)
- [ ] Adequate cooling if using hardware mode

**Software:**

- [ ] Pi accessible via SSH
- [ ] You have sudo privileges (or use PI_USER=root)
- [ ] Ports 8080, 50010, 50011 not in use
- [ ] SSH keys configured (recommended)

**Network:**

- [ ] Pi has static IP or DHCP reservation
- [ ] Firewall allows ports 8080, 50010, 50011
- [ ] No conflicts with existing services

## 🐛 Troubleshooting

### Deployment Issues

**SSH connection fails:**

```bash
# Test SSH manually
ssh pi@192.168.178.34

# Check SSH service on Pi
ping 192.168.178.34

# Verify SSH keys
ssh-copy-id pi@192.168.178.34
```

**Build fails:**

```bash
# Check logs
cat /tmp/wallbox_install.log

# Verify dependencies
ssh pi@192.168.178.34 'dpkg -l | grep -E "cmake|gcc|libmicrohttpd"'

# Retry with clean build
ssh pi@192.168.178.34 'cd ~/wallbox-src && rm -rf build && BUILD_MODE=production ./scripts/install.sh'
```

**Port conflicts:**

```bash
# Check if ports are in use
ssh pi@192.168.178.34 'sudo netstat -tulpn | grep -E "8080|50010|50011"'

# Kill conflicting processes
ssh pi@192.168.178.34 'sudo pkill -f wallbox_control'
```

### Runtime Issues

**Service not starting:**

```bash
# View detailed logs
ssh pi@192.168.178.34 'sudo journalctl -u wallbox -n 100 --no-pager'

# Check service status
ssh pi@192.168.178.34 'sudo systemctl status wallbox'

# Manual start for debugging
ssh pi@192.168.178.34 'cd ~/wallbox-src/build && ./wallbox_control_v3'
```

**API not responding:**

```bash
# Test locally on Pi
ssh pi@192.168.178.34 'curl http://localhost:8080/api/status'

# Check if process is running
ssh pi@192.168.178.34 'ps aux | grep wallbox_control'

# Check firewall
ssh pi@192.168.178.34 'sudo ufw status'
```

**GPIO permission issues (production mode):**

```bash
# Add user to gpio group
ssh pi@192.168.178.34 'sudo usermod -a -G gpio pi'

# Check GPIO access
ssh pi@192.168.178.34 'test -w /sys/class/gpio/export && echo OK || echo FAIL'

# Run with sudo (temporary)
ssh pi@192.168.178.34 'cd ~/wallbox-src/build && sudo ./wallbox_control_v3'
```

**CP Signal issues:**

```bash
# Check CP reader logs
ssh pi@192.168.178.34 'grep -i "CpSignal" /tmp/wallbox.log'

# Verify mode in config
ssh pi@192.168.178.34 'cat ~/wallbox-src/build/config.json | grep mode'

# Test simulator (development mode)
ssh pi@192.168.178.34 'cd ~/wallbox-src/build && ./simulator'
```

## 🎉 Success Indicators

**Deployment successful when you see:**

✅ **Build completed without errors:**

```
[ 95%] Building CXX object CMakeFiles/wallbox_control_v3.dir/src/main_v3.cpp.o
[100%] Linking CXX executable wallbox_control_v3
[100%] Built target wallbox_control_v3
```

✅ **Service started successfully:**

```
● wallbox.service - Wallbox Charging Station
   Loaded: loaded (/etc/systemd/system/wallbox.service; enabled)
   Active: active (running) since [timestamp]
```

✅ **API responds to status checks:**

```bash
curl http://<PI_IP>:8080/api/status
# Response:
{"state":"OFF","mode":"production","cp_signal":"Ready","uptime_seconds":45}
```

✅ **CP Signal System initialized:**

```bash
# In logs (journalctl -u wallbox):
Starting wallbox control with CP signal support...
Mode: production (GPIO pin 7)
CP Signal system initialized successfully
Current CP state: STATE_A (12V)
```

✅ **All test suite passed:**

```
All 11 tests passed! ✅
Test Duration: 8 seconds
System Status: Operational
```

✅ **GPIO permissions configured (production mode):**

```bash
ssh pi@<PI_IP> 'groups | grep gpio'
# Output: pi adm dialout gpio sudo audio video
```

✅ **Simulator communicating (development mode):**

```
Simulator started on UDP port 50010
Sending CP_STATE_A to wallbox_control
Received state update: IDLE
```

**Monitor system status:**

```bash
# Check real-time logs
ssh pi@<PI_IP> 'sudo journalctl -u wallbox -f'

# Check resource usage
ssh pi@<PI_IP> 'top -b -n 1 | grep wallbox_control'

# Verify CP signal readings
curl http://<PI_IP>:8080/api/status | grep cp_signal

# Run complete test suite
bash test_portable.sh <PI_IP>
```

---

**For more details, see:**

- `PORTABLE_TEST_RESULTS.md` - Complete test documentation (11 tests)
- `README.md` - Full architecture and features
- `CONFIG_GUIDE.md` - Configuration reference
- `scripts/deploy.sh` - Deployment script source
- `scripts/install.sh` - Installation script source

**Report issues:** Check logs in `/tmp/wallbox_install.log` and `sudo journalctl -u wallbox`

---

**Ready to deploy? Run:** `./scripts/deploy.sh <YOUR_PI_IP>`
