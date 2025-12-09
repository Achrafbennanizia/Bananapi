# Wallbox Controller - Complete System Overview

## System Architecture

This project provides a complete wallbox charging controller with two operational modes and a React web interface.

### Components

1. **Wallbox Controller (C++)** - Main control system

   - v1.0: Legacy procedural code
   - v2.0: SOLID architecture with design patterns
   - v2.0 + API: REST API for web/mobile apps

2. **GPIO Implementations**

   - **StubGpioController**: Simulator for development
   - **BananaPiGpioController**: Real hardware control via sysfs

3. **React Web App** - Modern web interface

   - Real-time status updates
   - One-click vehicle charging control
   - Works with both development and production modes

4. **UDP Simulator** - Protocol testing tool

---

## 🎯 Quick Start

### For Development (No Hardware)

```bash
# 1. Build the project
cd WallboxCtrl
mkdir -p build && cd build
cmake .. && make
cd ../..

# 2. Start development environment
./start-dev.sh

# 3. In a new terminal, start React app
cd wallbox-react-app
npm install  # First time only
npm start

# 4. Open browser: http://localhost:3000
```

**That's it!** You now have:

- ✅ Wallbox simulator running
- ✅ REST API server on port 8080
- ✅ React app on port 3000
- ✅ Full control from your browser

### For Production (Banana Pi Hardware)

```bash
# ⚠️ WARNING: This controls REAL hardware!

# 1. On Banana Pi, build the project
cd WallboxCtrl
mkdir -p build && cd build
cmake .. && make
cd ../..

# 2. Start production mode
sudo ./start-prod.sh

# 3. Access from any device on your network
# Open browser: http://<banana-pi-ip>:8080/api/status
```

---

## Operating Modes

### Development Mode (`WALLBOX_MODE=dev`)

**Purpose:** Safe testing and development

**Uses:**

- Simulated GPIO (no hardware)
- UDP protocol simulator
- REST API server
- React web app

**Perfect for:**

- Software development
- Testing new features
- Learning the system
- Demonstrating functionality

**Requirements:**

- Any computer (Mac, Linux, Windows)
- No special permissions
- No hardware

### Production Mode (`WALLBOX_MODE=prod`)

**Purpose:** Real wallbox control

**Uses:**

- Real Banana Pi GPIO pins
- Physical relay control
- Hardware LEDs and buttons
- REST API server (optional)

**Perfect for:**

- Actual EV charging
- Production deployment
- Real-world usage

**Requirements:**

- Banana Pi hardware
- Proper wiring
- sudo/root permissions
- Safety precautions

---

## System Components

### 1. Wallbox Controller API (`wallbox_control_api`)

**What it does:**

- Controls charging state machine
- Manages GPIO pins (real or simulated)
- Provides REST API endpoints
- Handles UDP protocol communication

**Endpoints:**

```
GET  /health                    - Health check
GET  /api/status                - Get current status
GET  /api/relay                 - Get relay status
POST /api/charging/start        - Start charging
POST /api/charging/stop         - Stop charging  ⭐
POST /api/charging/pause        - Pause charging
POST /api/charging/resume       - Resume charging
POST /api/wallbox/enable        - Enable wallbox
POST /api/wallbox/disable       - Disable wallbox
```

### 2. React Web App

**Features:**

- 🎨 Modern, responsive UI
- 📊 Real-time status (2-second updates)
- 🎮 Easy-to-use controls
- 🔴 **Big red STOP button**
- ✅ Connection status indicator
- 📱 Works on desktop and mobile

**Technology:**

- React 18
- Axios for API calls
- Modern CSS with gradients

### 3. UDP Simulator

**Purpose:** Simulate the wallbox protocol

**Features:**

- Responds to control messages
- Sends state updates
- Useful for development

**Ports:**

- Listen: 50010
- Send: 50011

### 4. GPIO Controllers

#### StubGpioController (Development)

- Logs all GPIO operations
- No hardware access
- Safe for testing
- Console output shows all actions

#### BananaPiGpioController (Production)

- Direct sysfs GPIO access
- Real hardware control
- Pin export/unexport
- Direction and value control

---

## File Structure

```
PJMT/
├── start-dev.sh              # Start development environment
├── start-prod.sh             # Start production environment
├── MODES_GUIDE.md            # Detailed mode documentation
├── SYSTEM_OVERVIEW.md        # This file
│
├── WallboxCtrl/              # Main controller
│   ├── CMakeLists.txt        # Build configuration
│   ├── include/              # Header files
│   │   ├── WallboxController.h
│   │   ├── StubGpioController.h
│   │   ├── BananaPiGpioController.h
│   │   └── HttpApiServer.h
│   ├── src/                  # Implementation
│   │   ├── main_v2_with_api.cpp    # Main with API
│   │   ├── WallboxController.cpp
│   │   ├── StubGpioController.cpp
│   │   ├── BananaPiGpioController.cpp
│   │   └── HttpApiServer.cpp
│   └── build/                # Build output
│       ├── wallbox_control_api     # Main executable
│       └── simulator               # UDP simulator
│
├── wallbox-react-app/        # React web interface
│   ├── package.json
│   ├── public/
│   │   └── index.html
│   ├── src/
│   │   ├── App.js            # Main component
│   │   ├── App.css           # Styling
│   │   └── api/
│   │       └── wallboxApi.js # API client
│   └── README.md
│
└── LibPubWallbox/            # Protocol library
```

---

## Use Cases

### 1. Software Development

```bash
# Use development mode
./start-dev.sh
cd wallbox-react-app && npm start

# Develop and test features safely
# No hardware required
# Full API access
```

### 2. User Acceptance Testing

```bash
# Demo to stakeholders
./start-dev.sh
# Open browser, demonstrate features
# Show all functionality without risk
```

### 3. Mobile App Development

```bash
# Run in dev mode
export WALLBOX_MODE=dev
cd WallboxCtrl/build
./wallbox_control_api

# Your mobile app connects to:
# http://localhost:8080/api/*
```

### 4. Production Deployment

```bash
# On Banana Pi
sudo ./start-prod.sh

# Monitor charging
curl http://localhost:8080/api/status

# Control remotely via React app
# (Configure API URL to Banana Pi IP)
```

### 5. Integration Testing

```bash
# Terminal 1: Simulator
cd WallboxCtrl/build
./simulator

# Terminal 2: Controller
export WALLBOX_MODE=dev
./wallbox_control_api

# Terminal 3: Test client
curl -X POST http://localhost:8080/api/charging/start
```

---

## Safety Features

### Development Mode

- ✅ No hardware access
- ✅ All operations logged
- ✅ Safe for experimentation
- ✅ Can't damage anything

### Production Mode

- ⚠️ Real hardware control
- ⚠️ Script asks for confirmation
- ⚠️ Requires sudo privileges
- ⚠️ Verifies GPIO availability
- ⚠️ Clear warning messages

---

## API Examples

### Check Health

```bash
curl http://localhost:8080/health
```

Response:

```json
{
  "status": "healthy",
  "service": "Wallbox Controller API",
  "version": "2.0.0"
}
```

### Get Status

```bash
curl http://localhost:8080/api/status
```

Response:

```json
{
  "state": "IDLE",
  "wallboxEnabled": true,
  "relayEnabled": false,
  "charging": false,
  "timestamp": 1702234567
}
```

### Stop Charging

```bash
curl -X POST http://localhost:8080/api/charging/stop
```

Response:

```json
{
  "success": true,
  "message": "Charging stopped",
  "newState": "IDLE"
}
```

---

## Configuration

### Environment Variables

**`WALLBOX_MODE`** - Operating mode

- `dev` or `development` → Development mode (default)
- `prod` or `production` → Production mode

**Example:**

```bash
# Development
export WALLBOX_MODE=dev

# Production
export WALLBOX_MODE=prod
```

### React App API URL

Edit `wallbox-react-app/src/api/wallboxApi.js`:

```javascript
// For local development
const API_BASE_URL = "http://localhost:8080";

// For remote Banana Pi
const API_BASE_URL = "http://192.168.1.100:8080";
```

### Port Configuration

Defined in `main_v2_with_api.cpp`:

```cpp
const int UDP_LISTEN_PORT = 50010;
const int UDP_SEND_PORT = 50011;
const int API_PORT = 8080;
```

### GPIO Pin Mapping

Defined in `WallboxController.h`:

```cpp
static constexpr int RELAY_ENABLE = 4;   // Main relay
static constexpr int LED_GREEN = 17;     // Ready
static constexpr int LED_YELLOW = 27;    // Charging
static constexpr int LED_RED = 22;       // Error
static constexpr int BUTTON = 23;        // User input
```

---

## Building from Source

### Prerequisites

- CMake 3.10+
- C++17 compiler (GCC/Clang)
- Node.js 14+ (for React app)

### Build Steps

```bash
# 1. Build C++ controller
cd WallboxCtrl
mkdir -p build && cd build
cmake ..
make

# Outputs:
# - wallbox_control      (v1.0 legacy)
# - wallbox_control_v2   (v2.0 SOLID)
# - wallbox_control_api  (v2.0 with API)
# - simulator            (UDP simulator)

# 2. Install React dependencies
cd ../../wallbox-react-app
npm install

# 3. Make scripts executable
cd ..
chmod +x start-dev.sh start-prod.sh
```

---

## Testing

### Quick Test (Development Mode)

```bash
# Terminal 1
./start-dev.sh

# Terminal 2
curl http://localhost:8080/health
curl http://localhost:8080/api/status
curl -X POST http://localhost:8080/api/charging/start
curl http://localhost:8080/api/status
curl -X POST http://localhost:8080/api/charging/stop
```

### React App Test

```bash
# Start dev environment
./start-dev.sh

# Start React app
cd wallbox-react-app && npm start

# Open browser: http://localhost:3000
# Click buttons, verify status updates
```

---

## Troubleshooting

See [MODES_GUIDE.md](MODES_GUIDE.md) for detailed troubleshooting.

### Common Issues

**Can't connect to API:**

- Check if `wallbox_control_api` is running
- Verify port 8080 is not in use
- Check firewall settings

**React app connection error:**

- Ensure API server is running first
- Check console for CORS errors
- Verify API_BASE_URL in wallboxApi.js

**Production mode fails:**

- Check GPIO sysfs: `ls /sys/class/gpio`
- Run with sudo: `sudo ./start-prod.sh`
- Verify pin permissions

---

## Documentation

- **MODES_GUIDE.md** - Comprehensive mode documentation
- **REACT_APP_API.md** - API reference and integration guide
- **README_V2.md** - v2.0 architecture documentation
- **wallbox-react-app/README.md** - React app documentation

---

## License

MIT

---

## Summary

This project provides:

1. ✅ Complete wallbox controller with SOLID architecture
2. ✅ Dual-mode operation (development/production)
3. ✅ REST API for remote control
4. ✅ Modern React web interface
5. ✅ Safe development environment
6. ✅ Production-ready hardware control
7. ✅ Comprehensive documentation

**Get started in 3 commands:**

```bash
./start-dev.sh
cd wallbox-react-app && npm start
# Open http://localhost:3000
```
