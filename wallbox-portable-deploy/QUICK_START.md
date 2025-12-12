# Wallbox Portable Deploy - Quick Test Guide

This directory contains a C++14 compatible, portable version of the Wallbox Controller ready for deployment to any Raspberry Pi, Banana Pi, or similar ARM SBC.

## 📦 What's in this package?

- ✅ All source code (src/_.cpp, include/_.h)
- ✅ C++14 compatible CMakeLists.txt
- ✅ Default configuration (config/config.json)
- ✅ Installation script (scripts/install.sh)
- ✅ Deployment script (scripts/deploy.sh)
- ✅ Local test script (scripts/test-local.sh)
- ✅ Remote test script (scripts/test-remote.sh)
- ✅ Comprehensive README.md

## 🚀 Quick Deployment

### Deploy to your Pi (easiest method):

```bash
# From this directory on your Mac/PC
./scripts/deploy.sh 192.168.178.34

# That's it! The script will:
# 1. Package the project
# 2. Copy to Pi via SSH
# 3. Install dependencies
# 4. Build with C++14 (using CMake by default)
# 5. Create systemd service
# 6. Start the wallbox
# 7. Test the installation
```

### Build with make (alternative):

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

### Manual installation (if deploying directly on Pi):

```bash
# Copy this entire folder to your Pi
scp -r ../wallbox-portable-deploy pi@192.168.178.34:~/

# SSH into Pi
ssh pi@192.168.178.34

# Run installer
cd wallbox-portable-deploy
sudo ./scripts/install.sh
```

## 🧪 Testing

### Test after deployment:

```bash
# From your computer
./scripts/test-remote.sh 192.168.178.34

# Or on the Pi itself
ssh pi@192.168.178.34
cd wallbox-portable-deploy
sudo ./scripts/test-local.sh
```

### Manual API test:

```bash
# Check status
curl http://192.168.178.34:8080/api/status

# View logs
ssh pi@192.168.178.34 'sudo journalctl -u wallbox -f'
```

## 📋 Features

All features from the main wallbox project are included:

- ✅ **Dual Mode**: Run API + Interactive simultaneously
- ✅ **HTTP API**: RESTful API on port 8080
- ✅ **UDP Communication**: ISO 15118 protocol (ports 50010/50011)
- ✅ **GPIO Control**: Direct hardware control
- ✅ **Simulator**: Test without hardware
- ✅ **Auto-restart**: Systemd service with watchdog
- ✅ **C++14 Compatible**: Works on all Pi platforms

## 🔧 Key Differences from Main Project

1. **C++14 Standard**: Uses `set(CMAKE_CXX_STANDARD 14)` instead of C++17
2. **Self-contained**: All dependencies specified in install script
3. **Platform Detection**: Auto-detects Raspberry Pi, Banana Pi, Orange Pi, etc.
4. **Automated Deployment**: One-command remote deployment
5. **Complete Testing**: Local and remote test suites included

## 📝 Configuration

Default config is in `config/config.json`:

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
  }
}
```

## 🎯 Platform Support

Tested on:

- ✅ Raspberry Pi (all models)
- ✅ Banana Pi M1, M2, M64
- ✅ Orange Pi One, PC, Plus
- ✅ Any Debian/Ubuntu ARM SBC

## 📚 Documentation

See `README.md` for comprehensive documentation including:

- Detailed installation steps
- API reference
- UDP communication guide
- Troubleshooting
- Integration examples
- Security considerations

## ⚡ Quick Commands

```bash
# Deploy to Pi
./scripts/deploy.sh <PI_IP>

# Test deployment
./scripts/test-remote.sh <PI_IP>

# View logs
ssh pi@<PI_IP> 'sudo journalctl -u wallbox -f'

# Restart service
ssh pi@<PI_IP> 'sudo systemctl restart wallbox'

# Check API
curl http://<PI_IP>:8080/api/status
```

## ✅ Pre-deployment Checklist

- [ ] Pi has network connection
- [ ] Pi is accessible via SSH
- [ ] You have sudo privileges
- [ ] Ports 8080, 50010, 50011 are not in use

## 🐛 Troubleshooting

**Deployment fails:**

```bash
# Check SSH connection
ssh pi@<PI_IP>

# Check if ports are free
ssh pi@<PI_IP> 'sudo netstat -tulpn | grep -E "8080|50010|50011"'
```

**Service not starting:**

```bash
# View logs
ssh pi@<PI_IP> 'sudo journalctl -u wallbox -n 50'

# Check dependencies
ssh pi@<PI_IP> 'dpkg -l | grep -E "libmicrohttpd|libcurl"'
```

**API not responding:**

```bash
# Test locally on Pi
ssh pi@<PI_IP> 'curl http://localhost:8080/api/status'

# Check firewall
ssh pi@<PI_IP> 'sudo ufw status'
```

## 🎉 Success Indicators

After deployment, you should see:

1. ✅ Service running: `sudo systemctl status wallbox` shows "active (running)"
2. ✅ API responding: `curl http://<PI_IP>:8080/api/status` returns JSON
3. ✅ Ports listening: `netstat -tulpn` shows 8080, 50010, 50011
4. ✅ No errors in logs: `journalctl -u wallbox -n 20`

---

**Ready to deploy? Run:** `./scripts/deploy.sh <YOUR_PI_IP>`

For detailed documentation, see `README.md`
