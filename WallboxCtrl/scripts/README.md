# WallboxCtrl Deployment Scripts

Scripts for deploying and managing the Wallbox Controller on remote systems.

## 📁 Scripts Overview

### `deploy.sh`

Automated deployment script for Raspberry Pi, Banana Pi, and other ARM SBCs.

**Features:**

- Automatic package creation with all source files
- SSH-based remote deployment
- Automatic dependency installation on target
- Remote compilation with optimizations
- System configuration (UDP networking)
- Optional systemd service setup

**Usage:**

```bash
# Basic deployment
./scripts/deploy.sh 192.168.178.34

# Custom user
PI_USER=root ./scripts/deploy.sh 192.168.178.34

# Development mode
BUILD_MODE=development ./scripts/deploy.sh bananapi

# Custom remote directory
REMOTE_DIR=/opt/wallbox ./scripts/deploy.sh 192.168.178.34
```

**Environment Variables:**

- `PI_USER` - SSH user (default: pi)
- `BUILD_MODE` - production or development (default: production)
- `REMOTE_DIR` - Remote installation directory (default: /home/$PI_USER/wallbox-src)

### `install.sh`

Local/remote installation and build script.

**Features:**

- Automatic dependency checking and installation
- CMake and Make build support
- Production and development builds
- Binary size optimization
- Systemd service creation

**Usage:**

```bash
# Local installation
./scripts/install.sh

# Development build
BUILD_MODE=development ./scripts/install.sh

# Custom install location
INSTALL_DIR=/opt/wallbox ./scripts/install.sh
```

**Build Modes:**

- `production` - Optimized (-O3), no debug symbols, small binaries
- `development` - Debug symbols (-g), detailed logging

## 🧪 Testing

### `test_wallbox.sh`

Comprehensive integration test suite.

**Tests:**

1. Process verification (wallbox_control_v3, simulator)
2. Network ports (8080, 50010, 50011)
3. Binary sizes
4. Configuration files
5. HTTP API endpoints
6. API functionality (status, enable, disable)
7. Relay control
8. State management
9. Error handling
10. Performance metrics
11. Log files

**Usage:**

```bash
# Run all tests
./test_wallbox.sh

# Must have wallbox and simulator running
cd build
./wallbox_control_v3 &
./simulator &
cd ..
./test_wallbox.sh
```

## 📦 Deployment Workflow

### Initial Setup

```bash
# 1. Deploy to Raspberry Pi
./scripts/deploy.sh 192.168.178.34

# 2. SSH to Pi and start services
ssh pi@192.168.178.34
cd ~/wallbox-src/build
./wallbox_control_v3 &

# 3. Start simulator on development machine
cd WallboxCtrl/build
./simulator
```

### Update Deployment

```bash
# Quick update (code changes only)
./scripts/deploy.sh 192.168.178.34

# Full rebuild
ssh pi@192.168.178.34 "cd ~/wallbox-src && rm -rf build && BUILD_MODE=production bash scripts/install.sh"
```

## 🔧 Configuration

### UDP Network Setup

The deployment automatically configures bidirectional UDP communication:

**Simulator → Wallbox (Pi):**

- Target: Pi IP address
- Port: 50010

**Wallbox (Pi) → Simulator (Mac):**

- Target: Mac IP address (auto-detected)
- Port: 50011

**Manual Configuration:**
Edit `config.json` on both sides:

```json
{
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "192.168.178.34",
    "api_port": 8080
  }
}
```

### Systemd Service (Optional)

```bash
# Create service
sudo bash scripts/install.sh --systemd

# Manage service
sudo systemctl start wallbox
sudo systemctl enable wallbox
sudo systemctl status wallbox
sudo journalctl -u wallbox -f
```

## 🐛 Troubleshooting

### Deployment Fails

```bash
# Check SSH connectivity
ssh pi@192.168.178.34 "echo Connection OK"

# Check sudo permissions
ssh pi@192.168.178.34 "sudo -n true" || echo "Sudo requires password"

# Use root user if needed
PI_USER=root ./scripts/deploy.sh 192.168.178.34
```

### Build Fails on Pi

```bash
# Check dependencies manually
ssh pi@192.168.178.34
sudo apt-get update
sudo apt-get install build-essential cmake libmicrohttpd-dev libcurl4-openssl-dev

# Check disk space
df -h

# Check compiler
gcc --version
cmake --version
```

### UDP Communication Issues

```bash
# Check ports on Pi
ssh pi@192.168.178.34 "lsof -i :50010"

# Check firewall
ssh pi@192.168.178.34 "sudo ufw status"

# Test UDP manually
echo "test" | nc -u 192.168.178.34 50010
```

## 📊 Performance

### Build Times (Raspberry Pi 4)

- Clean build: ~45 seconds
- Incremental: ~5 seconds

### Binary Sizes (Production)

- wallbox_control_v3: ~270 KB
- simulator: ~115 KB

### Resource Usage

- RAM: ~5 MB per process
- CPU: <5% idle, ~15% during charging

## 🔒 Security Notes

1. **SSH Keys:** Use key-based authentication instead of passwords
2. **Sudo:** Configure passwordless sudo for deployment user or use root
3. **Firewall:** Only expose required ports (8080 for API, 50010-50011 for UDP)
4. **Updates:** Keep system packages updated

## 📝 Version History

- **v4.1** - CP Signal System integration, clean terminal output
- **v4.0** - Full deployment automation with auto-dependency installation
- **v3.0** - Initial deployment scripts

## 📄 License

See main project LICENSE file.
