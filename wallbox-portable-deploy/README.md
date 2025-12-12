# Wallbox Controller - Portable Deployment Package

**C++14 Compatible | Universal Pi Support | Production Ready**

This is a self-contained, portable deployment package for the Wallbox Controller system. It's designed to work on any Raspberry Pi, Banana Pi, Orange Pi, or similar ARM single-board computer with minimal dependencies.

---

## 🎯 Features

- **Dual-Mode Operation**: API mode, Interactive mode, or both simultaneously
- **HTTP REST API**: Full-featured API on port 8080
- **UDP Communication**: ISO 15118 protocol support (ports 50010/50011)
- **GPIO Control**: Direct hardware control for charging stations
- **C++14 Compatible**: Works on older systems and newer platforms
- **Automated Deployment**: One-command installation and deployment
- **Comprehensive Testing**: Local and remote test suites included
- **Systemd Integration**: Run as a system service with auto-restart
- **Platform Detection**: Automatically adapts to your Pi model

---

## 📋 Requirements

### Hardware

- Any Raspberry Pi (1, 2, 3, 4, Zero, 5)
- Banana Pi (M1, M2, M3, M64)
- Orange Pi (One, PC, Plus, etc.)
- Any ARM SBC with GPIO support
- Minimum 512 MB RAM recommended
- Network connection (Ethernet or WiFi)

### Software

- Debian/Ubuntu-based OS (Raspbian, Ubuntu Server, Armbian, etc.)
- SSH access for remote deployment
- Root/sudo privileges

---

## 🚀 Quick Start

### Option 1: Local Installation (On the Pi itself)

```bash
# 1. Copy the package to your Pi
scp -r wallbox-portable-deploy pi@192.168.1.100:~/

# 2. SSH into the Pi
ssh pi@192.168.1.100

# 3. Run installation
cd wallbox-portable-deploy
sudo ./scripts/install.sh
```

### Option 2: Remote Deployment (From your computer)

```bash
# 1. Deploy to Pi (one command)
cd wallbox-portable-deploy
./scripts/deploy.sh 192.168.1.100

# That's it! The script handles everything:
# - Packages the project
# - Transfers via SSH
# - Installs dependencies
# - Builds the code
# - Creates systemd service
# - Starts the wallbox
# - Tests the installation
```

---

## 📁 Package Structure

```
wallbox-portable-deploy/
├── src/                    # Source files
│   ├── main_v3.cpp        # Main application
│   ├── HttpApiServer.cpp  # HTTP API implementation
│   ├── UdpCommunicator.cpp# UDP communication
│   ├── WallboxController.cpp # Core controller
│   └── ...
├── include/               # Header files
│   ├── HttpApiServer.h
│   ├── UdpCommunicator.h
│   ├── WallboxController.h
│   └── ...
├── config/
│   └── config.json        # Default configuration
├── scripts/
│   ├── install.sh         # Local installation script
│   ├── deploy.sh          # Remote deployment script
│   ├── test-local.sh      # Local testing script
│   └── test-remote.sh     # Remote testing script
├── build/                 # Build output (created during build)
├── docs/                  # Additional documentation
├── CMakeLists.txt        # CMake build configuration
├── Makefile              # Make build system
└── README.md             # This file
```

---

## 🔧 Installation Guide

### Build Methods

This package supports **two build systems**: CMake (default) and Make.

**CMake (recommended):**

- Modern build system
- Better dependency detection
- Used by deployment scripts

**Make (traditional):**

- Simpler, faster
- Direct control
- No CMake required

Both produce identical binaries!

### Step 1: Prepare Your Pi

**Initial Setup (if starting fresh):**

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Enable SSH (if not already enabled)
sudo systemctl enable ssh
sudo systemctl start ssh

# Configure network (optional)
sudo nmtui  # or edit /etc/network/interfaces
```

### Step 2: Transfer the Package

**From your computer:**

```bash
# Using SCP
scp -r wallbox-portable-deploy pi@<PI_IP>:~/

# Or using rsync (preserves permissions)
rsync -avz wallbox-portable-deploy/ pi@<PI_IP>:~/wallbox-portable-deploy/

# Or package as tarball
tar czf wallbox-deploy.tar.gz wallbox-portable-deploy/
scp wallbox-deploy.tar.gz pi@<PI_IP>:~/
ssh pi@<PI_IP> "tar xzf wallbox-deploy.tar.gz"
```

### Step 3: Install on Pi

**SSH into your Pi:**

```bash
ssh pi@<PI_IP>
cd wallbox-portable-deploy
```

**Run the installer:**

```bash
sudo ./scripts/install.sh
```

**What the installer does:**

1. Detects your platform (Raspberry Pi, Banana Pi, etc.)
2. Checks for required dependencies
3. Installs missing packages (cmake, gcc, libmicrohttpd, libcurl)
4. Creates installation directory (`/home/pi/wallbox-control`)
5. Copies files
6. Builds the project with CMake
7. Creates systemd service
8. Configures firewall rules
9. Starts the wallbox service

---

## 🛠️ Building the Project

You can build using either **CMake** or **Make**:

### Option 1: Build with Make (Simple & Fast)

```bash
cd ~/wallbox-control

# Build everything
make

# Or build specific targets
make wallbox      # Build wallbox only
make simulator    # Build simulator only

# Check dependencies first
make check-deps

# Install to system (optional)
sudo make install

# Clean build artifacts
make clean

# Show all available targets
make help
```

**Advantages:**

- Simple, traditional workflow
- Fast compilation
- No CMake required
- Direct Makefile control

### Option 2: Build with CMake (Recommended for Automation)

```bash
cd ~/wallbox-control
mkdir -p build && cd build

# Configure and build
cmake ..
make

# Or use cmake build command
cmake --build .

# Install (optional)
sudo make install
```

**Advantages:**

- Better dependency detection
- Cross-platform support
- Used by deployment scripts
- IDE integration

### Build Output

Both methods produce:

- `build/wallbox_control_v3` - Main wallbox controller
- `build/simulator` - ISO 15118 simulator

---

## 🏃 Running the Wallbox

### As a System Service (Recommended)

```bash
# Start service
sudo systemctl start wallbox

# Stop service
sudo systemctl stop wallbox

# Restart service
sudo systemctl restart wallbox

# Check status
sudo systemctl status wallbox

# View logs
sudo journalctl -u wallbox -f

# Enable auto-start on boot
sudo systemctl enable wallbox
```

### Manual Execution

**Using CMake build:**

```bash
cd ~/wallbox-control/build

# Run in API mode (default)
./wallbox_control_v3

# Run in interactive mode
./wallbox_control_v3 --interactive

# Run in dual mode (API + Interactive)
./wallbox_control_v3 --dual

# Run simulator
./simulator
```

**Using Make build:**

```bash
cd ~/wallbox-control
make              # Build everything
cd build
./wallbox_control_v3
```

---

## ⚙️ Configuration

Configuration file: `~/wallbox-control/build/config.json`

### Key Settings

```json
{
  "mode": "development",
  "api_port": 8080,
  "udp_port": 50010,
  "simulator_udp_port": 50011,
  "gpio": {
    "pp_pin": 17,
    "cp_pin": 27,
    "lock_pin": 22,
    "relay_pin": 23
  },
  "charging": {
    "max_current": 16,
    "voltage": 230,
    "phases": 1
  }
}
```

### Modify Configuration

```bash
# Edit config
nano ~/wallbox-control/build/config.json

# Restart service to apply
sudo systemctl restart wallbox
```

---

## 🧪 Testing

### Local Testing (Before Deployment)

```bash
# From your computer (before deploying)
cd wallbox-portable-deploy
./scripts/test-local.sh
```

**Tests performed:**

- Build directory validation
- Dependency checks
- Configuration validation
- Binary execution test
- Port availability
- API startup and response
- Simulator functionality
- UDP communication

### Remote Testing (After Deployment)

```bash
# From your computer (after deploying)
cd wallbox-portable-deploy
./scripts/test-remote.sh 192.168.1.100
```

**Tests performed:**

- SSH connectivity
- Installation verification
- Service status
- HTTP API endpoints
- UDP ports
- Configuration validation
- System logs analysis
- API endpoint testing
- GPIO configuration
- Performance metrics
- UDP communication
- Firewall rules

---

## 🌐 API Usage

### Base URL

```
http://<PI_IP>:8080/api
```

### Endpoints

#### Get Status

```bash
curl http://192.168.1.100:8080/api/status
```

Response:

```json
{
  "state": "IDLE",
  "current": 0,
  "voltage": 230,
  "power": 0,
  "energy": 0
}
```

#### Get Info

```bash
curl http://192.168.1.100:8080/api/info
```

#### Get Configuration

```bash
curl http://192.168.1.100:8080/api/config
```

#### Start Charging

```bash
curl -X POST http://192.168.1.100:8080/api/start \
  -H "Content-Type: application/json" \
  -d '{"current": 16}'
```

#### Stop Charging

```bash
curl -X POST http://192.168.1.100:8080/api/stop
```

---

## 🔌 UDP Communication

### Wallbox UDP Interface

**Port:** 50010  
**Protocol:** ISO 15118

### Simulator UDP Interface

**Port:** 50011  
**Protocol:** ISO 15118

### Send UDP Command

```bash
# Using netcat
echo "START_CHARGING" | nc -u 192.168.1.100 50010

# Using Python
python3 << EOF
import socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(b"START_CHARGING", ("192.168.1.100", 50010))
EOF
```

---

## 🐛 Troubleshooting

### Service Won't Start

```bash
# Check service status
sudo systemctl status wallbox

# View detailed logs
sudo journalctl -u wallbox -n 100 --no-pager

# Check if ports are in use
sudo netstat -tulpn | grep -E '8080|50010|50011'

# Kill conflicting processes
sudo pkill wallbox_control
sudo systemctl start wallbox
```

### API Not Responding

```bash
# Check if service is running
ps aux | grep wallbox

# Check firewall
sudo ufw status
sudo ufw allow 8080
sudo ufw allow 50010/udp
sudo ufw allow 50011/udp

# Test locally on Pi
curl http://localhost:8080/api/status

# Check network
ping 192.168.1.100
```

### Build Errors

**CMake build:**

```bash
# Reinstall dependencies
sudo apt update
sudo apt install -y build-essential cmake libmicrohttpd-dev libcurl4-openssl-dev

# Clean and rebuild
cd ~/wallbox-control
rm -rf build
mkdir build && cd build
cmake ..
make
```

**Make build:**

```bash
# Reinstall dependencies
sudo apt update
sudo apt install -y build-essential libmicrohttpd-dev libcurl4-openssl-dev

# Clean and rebuild
cd ~/wallbox-control
make clean
make
```

### GPIO Permission Errors

```bash
# Add user to gpio group
sudo usermod -a -G gpio $USER

# For Raspberry Pi
sudo usermod -a -G gpio pi

# Reboot
sudo reboot
```

### UDP Communication Issues

```bash
# Check if ports are listening
sudo netstat -ulpn | grep 50010

# Test UDP locally
echo "test" | nc -u localhost 50010

# Check firewall
sudo ufw allow 50010/udp
sudo ufw allow 50011/udp

# Verify config
cat ~/wallbox-control/build/config.json | grep udp
```

---

## 🔒 Security Considerations

### Firewall Configuration

```bash
# Enable firewall
sudo ufw enable

# Allow SSH
sudo ufw allow 22

# Allow Wallbox ports
sudo ufw allow 8080/tcp
sudo ufw allow 50010/udp
sudo ufw allow 50011/udp

# Check status
sudo ufw status
```

### Restrict API Access

**Edit config.json:**

```json
{
  "api": {
    "bind_address": "127.0.0.1", // Local only
    "port": 8080
  }
}
```

**Use nginx as reverse proxy:**

```nginx
location /api/ {
    proxy_pass http://localhost:8080/api/;
    # Add authentication here
}
```

---

## 📊 Monitoring

### View Real-time Logs

```bash
# Follow wallbox logs
sudo journalctl -u wallbox -f

# Last 100 lines
sudo journalctl -u wallbox -n 100

# Logs since boot
sudo journalctl -u wallbox -b

# Logs from last hour
sudo journalctl -u wallbox --since "1 hour ago"
```

### System Resource Usage

```bash
# CPU and memory
top -p $(pgrep wallbox_control)

# Detailed process info
ps aux | grep wallbox

# Network connections
sudo netstat -tulpn | grep wallbox
```

### API Monitoring Script

```bash
#!/bin/bash
while true; do
  STATUS=$(curl -s http://localhost:8080/api/status)
  echo "$(date): $STATUS"
  sleep 5
done
```

---

## 🔄 Updates and Maintenance

### Update Wallbox

```bash
# Stop service
sudo systemctl stop wallbox

# Backup config
cp ~/wallbox-control/build/config.json ~/config.backup

# Deploy new version
./scripts/deploy.sh 192.168.1.100

# Restore config if needed
cp ~/config.backup ~/wallbox-control/build/config.json

# Restart
sudo systemctl start wallbox
```

### Backup Configuration

```bash
# Create backup
tar czf wallbox-backup-$(date +%Y%m%d).tar.gz ~/wallbox-control/build/config.json

# Restore backup
tar xzf wallbox-backup-20241201.tar.gz -C /
```

---

## 📝 Platform-Specific Notes

### Raspberry Pi

- **GPIO Library:** Uses standard Linux GPIO sysfs
- **Performance:** Pi 3/4 recommended for best performance
- **WiFi:** Built-in on Pi 3/4/Zero W

### Banana Pi

- **GPIO Pins:** May differ from Raspberry Pi
- **Check pinout:** `gpio readall` or consult Banana Pi docs
- **Adjust config.json:** Update GPIO pin numbers

### Orange Pi

- **GPIO Access:** May require `orangepi-gpio` package
- **Install:** `sudo apt install orangepi-gpio`
- **Pin mapping:** Consult Orange Pi documentation

---

## 🤝 Integration Examples

### Python Client

```python
import requests
import json

class WallboxClient:
    def __init__(self, host, port=8080):
        self.base_url = f"http://{host}:{port}/api"

    def get_status(self):
        response = requests.get(f"{self.base_url}/status")
        return response.json()

    def start_charging(self, current=16):
        data = {"current": current}
        response = requests.post(f"{self.base_url}/start", json=data)
        return response.json()

    def stop_charging(self):
        response = requests.post(f"{self.base_url}/stop")
        return response.json()

# Usage
wallbox = WallboxClient("192.168.1.100")
status = wallbox.get_status()
print(f"State: {status['state']}")
```

### Node.js Client

```javascript
const axios = require("axios");

class WallboxClient {
  constructor(host, port = 8080) {
    this.baseUrl = `http://${host}:${port}/api`;
  }

  async getStatus() {
    const response = await axios.get(`${this.baseUrl}/status`);
    return response.data;
  }

  async startCharging(current = 16) {
    const response = await axios.post(`${this.baseUrl}/start`, { current });
    return response.data;
  }

  async stopCharging() {
    const response = await axios.post(`${this.baseUrl}/stop`);
    return response.data;
  }
}

// Usage
const wallbox = new WallboxClient("192.168.1.100");
wallbox.getStatus().then((status) => {
  console.log(`State: ${status.state}`);
});
```

---

## 📚 Additional Resources

### Documentation

- See `docs/` folder for detailed architecture and API documentation
- Configuration guide: `CONFIG_GUIDE.md`
- Interactive mode guide: `INTERACTIVE_GUIDE.md`

### Support

- Check logs: `sudo journalctl -u wallbox -n 100`
- Test scripts: `./scripts/test-remote.sh <IP>`
- System info: `uname -a && free -h && df -h`

### Development

- Source code: `src/` and `include/`
- CMake build: `CMakeLists.txt`
- Local testing: `./scripts/test-local.sh`

---

## ✅ Deployment Checklist

Before deployment:

- [ ] Pi is accessible via SSH
- [ ] Network is configured (static IP recommended)
- [ ] You have sudo/root access
- [ ] Ports 8080, 50010, 50011 are available

During deployment:

- [ ] Run `./scripts/deploy.sh <PI_IP>`
- [ ] Wait for installation to complete
- [ ] Check for any error messages

After deployment:

- [ ] Run `./scripts/test-remote.sh <PI_IP>`
- [ ] Verify API: `curl http://<PI_IP>:8080/api/status`
- [ ] Check service: `ssh pi@<PI_IP> 'sudo systemctl status wallbox'`
- [ ] View logs: `ssh pi@<PI_IP> 'sudo journalctl -u wallbox -n 20'`
- [ ] Configure auto-start: `ssh pi@<PI_IP> 'sudo systemctl enable wallbox'`

---

## 📄 License

See main project repository for license information.

---

## 🎉 You're Done!

Your Wallbox Controller should now be running on your Pi!

**Quick links:**

- API: `http://<PI_IP>:8080/api/status`
- Logs: `ssh pi@<PI_IP> 'sudo journalctl -u wallbox -f'`
- Restart: `ssh pi@<PI_IP> 'sudo systemctl restart wallbox'`

**Need help?** Check the Troubleshooting section above or review the logs.

---

_Last updated: 2024 | Wallbox Controller v3 | C++14 Portable Edition_
