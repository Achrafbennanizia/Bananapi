# Wallbox Controller - Modes and Deployment Guide

## Overview

The Wallbox Controller supports two operational modes:

1. **Development Mode** - Uses simulator, works with React app and website
2. **Production Mode** - Uses real Banana Pi GPIO pins

## 🔧 Development Mode

### What it includes:

- ✅ GPIO Simulator (no real hardware needed)
- ✅ UDP Simulator for protocol testing
- ✅ REST API server for React app
- ✅ Safe for testing on any computer
- ✅ Full integration with React web app

### How to use:

#### Option 1: Automatic (Recommended)

```bash
./start-dev.sh
```

This script will:

1. Start the UDP simulator
2. Start the API server in development mode
3. Show instructions for starting the React app

#### Option 2: Manual

**Terminal 1 - Start Simulator:**

```bash
cd build/bin
./simulator
```

**Terminal 2 - Start API Server:**

```bash
export WALLBOX_MODE=dev
cd build/bin
./wallbox_control_api
```

**Terminal 3 - Start React App:**

```bash
cd web/react-app
npm install  # First time only
npm start
```

### Access Points:

- **React App**: http://localhost:3000
- **API Server**: http://localhost:8080
- **API Status**: http://localhost:8080/api/status

### Development Workflow:

1. Start all services using `./start-dev.sh`
2. Open React app in browser
3. Control wallbox from the web interface
4. Simulator responds to all commands
5. No real hardware is involved

---

## ⚡ Production Mode (Banana Pi Hardware)

### ⚠️ WARNING: This mode controls REAL GPIO pins!

### What it includes:

- ⚡ Real GPIO pin control via sysfs
- ⚡ Direct hardware interaction
- ⚡ REST API for remote control
- ⚠️ Can damage hardware if misconfigured

### Prerequisites:

1. Banana Pi hardware with GPIO support
2. GPIO sysfs enabled (`/sys/class/gpio` exists)
3. Proper wiring of relay and LEDs
4. Permissions to access GPIO

### GPIO Pin Mapping:

```
Pin 4  - Relay Control (OUTPUT)
Pin 17 - Green LED (OUTPUT)
Pin 27 - Yellow LED (OUTPUT)
Pin 22 - Red LED (OUTPUT)
Pin 23 - Button Input (INPUT)
```

### How to use:

#### Option 1: Automatic (Recommended)

```bash
sudo ./start-prod.sh
```

The script will:

1. Verify GPIO hardware availability
2. Ask for confirmation (safety check)
3. Start API server with real GPIO

#### Option 2: Manual

```bash
export WALLBOX_MODE=prod
cd build/bin
sudo ./wallbox_control_api
```

### Access Points:

- **API Server**: http://<banana-pi-ip>:8080
- **API Status**: http://<banana-pi-ip>:8080/api/status

### Control Options:

#### 1. React App (Remote Control)

```bash
# On your computer (not Banana Pi):
cd web/react-app
# Edit src/api/wallboxApi.js:
# Change: const API_BASE_URL = 'http://192.168.1.xxx:8080';
npm start
```

#### 2. curl Commands

```bash
# Start charging
curl -X POST http://localhost:8080/api/charging/start

# Stop charging
curl -X POST http://localhost:8080/api/charging/stop

# Get status
curl http://localhost:8080/api/status
```

#### 3. Your own application

Use the REST API from any application that supports HTTP.

---

## Environment Variables

### `WALLBOX_MODE`

Controls which GPIO implementation to use:

- `dev` or `development` → Uses stub GPIO (simulator)
- `prod` or `production` → Uses real Banana Pi GPIO pins

**Default:** `dev` (safe default)

### Examples:

```bash
# Development mode
WALLBOX_MODE=dev ./wallbox_control_api

# Production mode
WALLBOX_MODE=prod ./wallbox_control_api
```

---

## Mode Comparison

| Feature         | Development Mode | Production Mode             |
| --------------- | ---------------- | --------------------------- |
| GPIO            | Simulated        | Real hardware               |
| Hardware needed | None             | Banana Pi                   |
| Safe to test    | ✅ Yes           | ⚠️ No - can damage hardware |
| React app       | ✅ Works         | ✅ Works (remote)           |
| UDP Simulator   | ✅ Required      | ❌ Not needed               |
| Permissions     | Normal user      | `sudo` required             |
| Network         | localhost        | LAN/WAN accessible          |

---

## Quick Reference

### Development Mode

```bash
# Start everything
./start-dev.sh

# Then in another terminal:
cd web/react-app && npm start

# Open browser: http://localhost:3000
```

### Production Mode

```bash
# On Banana Pi
sudo ./start-prod.sh

# Control from React app on another computer
# Or use curl/REST API
```

---

## Troubleshooting

### Development Mode

**Problem:** "Cannot connect to wallbox controller"

```bash
# Check if API server is running:
ps aux | grep wallbox_control_api

# Check if simulator is running:
ps aux | grep simulator

# Restart:
./start-dev.sh
```

**Problem:** React app can't connect

```bash
# Verify API is accessible:
curl http://localhost:8080/health

# Should return: {"status":"healthy",...}
```

### Production Mode

**Problem:** "GPIO sysfs not available"

```bash
# Check GPIO support:
ls -l /sys/class/gpio

# If missing, GPIO is not enabled in kernel
```

**Problem:** Permission denied

```bash
# Run with sudo:
sudo ./start-prod.sh

# Or add user to gpio group:
sudo usermod -a -G gpio $USER
```

**Problem:** Pin export failed

```bash
# Manual pin export:
echo 4 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio4/direction

# Check if it worked:
ls -l /sys/class/gpio/gpio4
```

---

## Architecture

```
Development Mode:
┌─────────────────┐      ┌──────────────┐      ┌─────────────────┐
│   React App     │─────▶│  API Server  │─────▶│  Stub GPIO      │
│  (port 3000)    │◀─────│  (port 8080) │◀─────│  (Simulator)    │
└─────────────────┘      └──────────────┘      └─────────────────┘
                                │
                                ▼
                         ┌──────────────┐
                         │ UDP Simulator│
                         │ (ports 50010)│
                         └──────────────┘

Production Mode:
┌─────────────────┐      ┌──────────────┐      ┌─────────────────┐
│   React App     │─────▶│  API Server  │─────▶│ BananaPi GPIO   │
│  (remote)       │◀─────│  (port 8080) │◀─────│ (Real Hardware) │
└─────────────────┘      └──────────────┘      └─────────────────┘
                                                        │
                                                        ▼
                                                 ┌──────────────┐
                                                 │ Relay, LEDs, │
                                                 │   Buttons    │
                                                 └──────────────┘
```

---

## Safety Checklist

Before running in production mode:

- [ ] Verified correct GPIO pin numbers
- [ ] Checked wiring connections
- [ ] Relay is properly rated for load
- [ ] Power supply is adequate
- [ ] Fuses/protection in place
- [ ] Tested in development mode first
- [ ] Understand what each API endpoint does
- [ ] Have emergency stop mechanism
- [ ] Monitoring/logging enabled

---

## Next Steps

### Development:

1. ✅ Install React app dependencies: `cd web/react-app && npm install`
2. ✅ Start dev environment: `./start-dev.sh`
3. ✅ Start React app: `npm start` in wallbox-react-app folder
4. ✅ Test all features in simulator

### Production:

1. ⚠️ Complete hardware wiring
2. ⚠️ Verify GPIO pins with multimeter
3. ⚠️ Test with low-power load first
4. ⚠️ Run `sudo ./start-prod.sh`
5. ⚠️ Monitor carefully during first use

---

## Support

For issues or questions:

- Check logs: `/tmp/wallbox_*.log` (development mode)
- Verify mode: Look for "DEVELOPMENT 🔧" or "PRODUCTION ⚡" in output
- Test API: `curl http://localhost:8080/health`
- Check GPIO: `cat /sys/class/gpio/gpio4/value` (production only)
