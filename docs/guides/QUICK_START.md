# Quick Start Guide - Wallbox Control System v4.1

## 🚀 Quick Installation

### Build from Source

```bash
cd <PROJECT_ROOT>

# Create build directory
mkdir -p build && cd build

# Configure (choose one):
cmake ..                              # Production mode (default)
BUILD_MODE=development cmake ..       # Development mode
BUILD_MODE=debug cmake ..             # Debug mode with sanitizers

# Build all targets
make -j$(nproc)                       # Linux
make -j$(sysctl -n hw.ncpu)          # macOS

# Built executables are in: build/bin/
```

### Quick Build Script (Alternative)

```bash
cd <PROJECT_ROOT>

# Production build
./scripts/build.sh

# Development build with verbose output
BUILD_MODE=development ./scripts/build.sh
```

## 🎯 Running the System

### Terminal 1 - Wallbox Controller (v3.0 - Latest)

```bash
cd <PROJECT_ROOT>/build/bin
./wallbox_control_v3
```

**Features:**

- ✅ HTTP REST API on port 8080
- ✅ UDP communication on port 50010
- ✅ Clean terminal output
- ✅ Production-ready for BananaPi

### Terminal 2 - ISO 15118 Simulator

```bash
cd <PROJECT_ROOT>/build/bin
./simulator
```

**Available Commands:**

- `on` - Turn main contactor ON
- `off` - Turn main contactor OFF
- `idle` - Set state to IDLE
- `ready` - Set state to READY
- `charge` - Set state to CHARGING
- `stop` - Set state to STOP
- `status` - Show current status
- `getudp` - Show UDP configuration
- `setudp <ip> <in> <out>` - Change UDP config
- `help` - Show help
- `quit` - Exit

**Features:**

- ✅ Clean terminal output (commands & wallbox feedback only)
- ✅ All UDP traffic logged to `/tmp/wallbox_simulator.log`
- ✅ Auto-loads config from `config.json`
- ✅ Runtime UDP reconfiguration

## 🧪 Test Sequence

### 1. Check System Status

**Wallbox Terminal:**

```
> (System shows initial state automatically)
```

**Simulator Terminal:**

```
> status
--- Current Status ---
Main Contactor: OFF
Charging State: idle
UDP Address: 127.0.0.1
UDP In Port: 50011
UDP Out Port: 50010
```

### 2. Start Charging Sequence

**Simulator Terminal:**

```
> on
✓ Main contactor ON

[WALLBOX] ⚡ Contactor ON

> charge
✓ State: CHARGING
```

**Wallbox responds** with feedback messages showing state changes.

### 3. Test HTTP API

```bash
# Check status via HTTP API
curl http://localhost:8080/api/status

# Enable wallbox
curl -X POST http://localhost:8080/api/enable

# Disable wallbox
curl -X POST http://localhost:8080/api/disable

# Get current state
curl http://localhost:8080/api/state
```

## 📦 Deployment to BananaPi

### Using Deployment Script

```bash
cd <PROJECT_ROOT>

# Interactive deployment (prompts for mode)
./scripts/deploy/deploy.sh <API_HOST>

# Direct production deployment
./scripts/deploy/deploy.sh <API_HOST> --mode production

# Development deployment with custom user
PI_USER=admin ./scripts/deploy/deploy.sh <API_HOST> --mode development
```

### Manual Deployment

```bash
# Build for production
cd <PROJECT_ROOT>
BUILD_MODE=production cmake -B build && make -C build -j$(nproc)

# Copy to BananaPi
scp build/bin/wallbox_control_v3 pi@<API_HOST>:~/
scp build/bin/simulator pi@<API_HOST>:~/
scp config/*.json pi@<API_HOST>:~/

# SSH and run
ssh pi@<API_HOST>
./wallbox_control_v3
```

## 📊 View Logs

### Simulator Log (All UDP Traffic)

```bash
tail -f /tmp/wallbox_simulator.log
```

### Wallbox Log

```bash
tail -f /tmp/wallbox_v3.log

# Or in production with systemd:
journalctl -u wallbox -f
```

## 🔧 Configuration

Edit `config.json` in your preferred location:

```json
{
  "mode": "development",
  "gpio": {
    "type": "stub",
    "relay_pin": 21
  },
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "127.0.0.1"
  }
}
```

**Modes:**

- `development` - Uses stub GPIO (no hardware required)
- `production` - Uses real BananaPi GPIO

**GPIO Types:**

- `stub` - Simulated GPIO for testing
- `bananapi` - Real hardware GPIO access

## 🆘 Troubleshooting

### Build Issues

```bash
# Clean build
cd <PROJECT_ROOT>
rm -rf build && mkdir build && cd build
cmake .. && make -j$(sysctl -n hw.ncpu)

# Check dependencies
cmake .. 2>&1 | grep -i "not found"

# Verify include paths
cmake .. 2>&1 | grep -i "include"
```

### Processes Won't Start

```bash
# Kill existing processes
pkill -9 wallbox_control
pkill -9 simulator

# Check ports
lsof -i :50010
lsof -i :50011
```

### UDP Communication Issues

```bash
# Simulator: check current UDP config
> getudp

# Simulator: change to localhost
> setudp 127.0.0.1 50011 50010

# Verify config.json
cat <PROJECT_ROOT>/config/development.json
```

### GPIO Access Issues (BananaPi)

```bash
# Check GPIO permissions
ls -l /sys/class/gpio

# Run with sudo (production)
sudo ./wallbox_control_v3

# Use stub GPIO for testing
# Set "type": "stub" in config.json
```

## 📚 More Information

- **Full Documentation**: See `<PROJECT_ROOT>/docs` directory
- **API Reference**: `docs/api/API_REFERENCE.md`
- **Installation Guide**: `docs/guides/INSTALLATION_GUIDE.md`
- **Architecture**: `docs/architecture/ARCHITECTURE_V3.md`
- **Build System**: Root `CMakeLists.txt` for modern build
- **Legacy Build**: `WallboxCtrl/CMakeLists.txt` for compatibility

## 📋 Available Executables

After building, you'll find in `build/bin/`:

- `wallbox_control_v3` - **Latest version** (full features, HTTP API, CP signals)
- `wallbox_control_v4` - Previous version (simple relay control)
- `wallbox_control_v2` - Older version (SOLID architecture)
- `wallbox_control_v1` - Original version (legacy)
- `simulator` - ISO 15118 stack simulator

**Recommended**: Use `wallbox_control_v3` for production deployments.
