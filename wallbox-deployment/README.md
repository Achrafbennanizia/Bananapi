# Wallbox System - Banana Pi Deployment Package

**Version**: 3.0  
**Target**: Banana Pi (ARM Linux)  
**Date**: December 10, 2025

## 📦 Package Contents

This deployment package contains everything needed to run the wallbox control system on a Banana Pi.

```
wallbox-deployment/
├── bin/                    # Executables
│   ├── wallbox_control_v3  # Main wallbox controller
│   └── simulator           # ISO 15118 simulator (for testing)
│
├── config/                 # Configuration files
│   └── config.json         # Runtime configuration
│
├── scripts/                # Control scripts
│   ├── start-wallbox.sh    # Start wallbox service
│   ├── stop-wallbox.sh     # Stop wallbox service
│   ├── start-simulator.sh  # Start simulator (testing)
│   └── install-service.sh  # Install as systemd service
│
├── lib/                    # Required libraries (if needed)
│
└── docs/                   # Essential documentation
    ├── QUICK_START.md      # Quick setup guide
    ├── CONFIG_GUIDE.md     # Configuration reference
    └── COMMANDS.md         # Command reference
```

## 🚀 Quick Setup on Banana Pi

### 1. Copy to Banana Pi

```bash
# On your Mac, transfer the entire directory
scp -r wallbox-deployment/ pi@<banana-pi-ip>:~/

# Or use rsync for faster transfer
rsync -avz wallbox-deployment/ pi@<banana-pi-ip>:~/wallbox/
```

### 2. Install on Banana Pi

```bash
# SSH into Banana Pi
ssh pi@<banana-pi-ip>

# Navigate to deployment directory
cd ~/wallbox-deployment

# Make scripts executable
chmod +x scripts/*.sh
chmod +x bin/*

# Run installation
sudo ./scripts/install-service.sh
```

### 3. Start the System

```bash
# Start wallbox service
./scripts/start-wallbox.sh

# Or start as systemd service
sudo systemctl start wallbox
sudo systemctl enable wallbox  # Auto-start on boot
```

## 📋 Requirements

### Hardware
- Banana Pi (any model with GPIO support)
- Network connection (for UDP communication)
- USB/Serial connection (optional for debugging)

### Software
- Linux kernel with GPIO support
- WiringPi library (or compatible)
- Network stack (UDP ports 50010, 50011)

### Optional
- HTTP API support (port 8080)
- React web interface (port 3000)

## 🔧 Configuration

Edit `config/config.json` to customize:

```json
{
  "gpio_mode": "bananapi",
  "udp_listen_port": 50010,
  "udp_send_port": 50011,
  "api_enabled": true,
  "api_port": 8080,
  "log_level": "info"
}
```

## 📡 Network Ports

| Port  | Protocol | Purpose                |
|-------|----------|------------------------|
| 50010 | UDP      | Wallbox receives       |
| 50011 | UDP      | Wallbox sends          |
| 8080  | TCP/HTTP | REST API (optional)    |
| 3000  | TCP/HTTP | Web UI (optional)      |

## 🧪 Testing

```bash
# Start simulator (in one terminal)
./bin/simulator

# Start wallbox (in another terminal)
./bin/wallbox_control_v3

# Test commands in simulator
# Type 'help' for available commands
```

## 🔒 Security

- Run as non-root user when possible
- Configure firewall rules for UDP/HTTP ports
- Use secure network for production
- Disable API in production if not needed

## 📚 Documentation

- **Quick Start**: docs/QUICK_START.md
- **Configuration**: docs/CONFIG_GUIDE.md
- **Commands**: docs/COMMANDS.md
- **Full Documentation**: See main project repository

## 🆘 Troubleshooting

### Wallbox won't start
```bash
# Check if binary is executable
ls -l bin/wallbox_control_v3

# Check config file
cat config/config.json

# Check logs
journalctl -u wallbox -f
```

### GPIO errors
```bash
# Check GPIO permissions
ls -l /sys/class/gpio

# Add user to gpio group
sudo usermod -a -G gpio $USER
```

### Network errors
```bash
# Check UDP ports
sudo netstat -ulnp | grep -E '50010|50011'

# Test UDP connectivity
nc -u localhost 50010
```

## 🔄 Updates

To update the system:

```bash
# Stop the service
./scripts/stop-wallbox.sh

# Replace binaries
cp new_wallbox_control_v3 bin/

# Restart service
./scripts/start-wallbox.sh
```

## 📝 Version Information

- **Version**: 3.0
- **Build Date**: December 10, 2025
- **Architecture**: ARM Linux
- **Compiler**: GCC 11+
- **C++ Standard**: C++17

## 📄 License

See main project repository for license information.

---

**For complete documentation, visit the main project repository**
