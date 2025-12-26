# Wallbox Controller - Mode Guide

**Version:** 4.0  
**Date:** December 26, 2025  
**Application:** `wallbox_control_v4`

---

## 📋 Overview

The Wallbox Controller supports multiple operating modes and configurations. This guide explains each mode, its features, and how to combine different options.

---

## 1. Configuration Modes

### 1.1 Production Mode

**Purpose:** Run with real hardware GPIO on BananaPi M5

**Command:**

```bash
./wallbox_control_v4 production
./wallbox_control_v4 prod
```

**Features:**

- ✅ Uses real GPIO pins via sysfs
- ✅ Controls actual LEDs and relay
- ✅ Reads physical button input
- ✅ Full hardware integration
- ⚠️ Requires BananaPi M5 hardware

**Config File:** `config/production.json`

**GPIO Pins Used:**
| Pin | GPIO | Function |
|-----|------|----------|
| 21 | 586 | Relay |
| 22 | 579 | Red LED |
| 10 | 590 | Yellow LED |
| 24 | 587 | Green LED |
| 23 | 588 | Button |

---

### 1.2 Development Mode

**Purpose:** Run in simulation mode for testing without hardware

**Command:**

```bash
./wallbox_control_v4 development
./wallbox_control_v4 dev
./wallbox_control_v4              # Default if no config specified
```

**Features:**

- ✅ Uses StubGpioController (simulated GPIO)
- ✅ No hardware required
- ✅ Safe for testing on any machine
- ✅ Logs GPIO operations to console
- ✅ Works on macOS, Linux, Windows

**Config File:** `config/development.json`

---

### 1.3 Test Mode

**Purpose:** Run with test configuration for automated testing

**Command:**

```bash
./wallbox_control_v4 test
```

**Features:**

- ✅ Uses test-specific settings
- ✅ Suitable for CI/CD pipelines
- ✅ Isolated test configuration

**Config File:** `config/test.json`

---

### 1.4 Custom Config

**Purpose:** Use a custom configuration file

**Command:**

```bash
./wallbox_control_v4 /path/to/custom.json
./wallbox_control_v4 myconfig.json
```

**Features:**

- ✅ Any `.json` file path accepted
- ✅ Full flexibility for custom setups

---

## 2. Interface Modes

### 2.1 API Server Mode (Default)

**Purpose:** REST API server only, no console interaction

**Command:**

```bash
./wallbox_control_v4 [config]
```

**Features:**

- ✅ HTTP REST API on port 8080
- ✅ Can run as background service
- ✅ Ideal for production deployment
- ❌ No interactive console

**Use Case:** Headless server, service deployment, React app backend

**REST API Endpoints:**
| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/status` | Get wallbox status |
| GET | `/api/relay` | Get relay state |
| GET | `/health` | Health check |
| POST | `/api/wallbox/enable` | Enable wallbox |
| POST | `/api/wallbox/disable` | Disable wallbox |
| POST | `/api/charging/start` | Start charging |
| POST | `/api/charging/stop` | Stop charging |
| POST | `/api/charging/pause` | Pause charging |
| POST | `/api/charging/resume` | Resume charging |

---

### 2.2 Interactive Mode

**Purpose:** Terminal console only, no REST API

**Command:**

```bash
./wallbox_control_v4 [config] --interactive
./wallbox_control_v4 [config] -i
```

**Features:**

- ✅ Interactive command console
- ✅ Real-time status display
- ✅ Direct command input
- ❌ No REST API (port 8080 not used)

**Use Case:** Direct testing, debugging, manual control

**Console Commands:**
| Command | Description |
|---------|-------------|
| `help` | Show all available commands |
| `status` | Display current wallbox status |
| `enable` | Enable the wallbox |
| `disable` | Disable the wallbox |
| `start` | Start charging session |
| `stop` | Stop charging session |
| `pause` | Pause current charging |
| `resume` | Resume paused charging |
| `relay on` | Turn relay ON |
| `relay off` | Turn relay OFF |
| `led green on` | Turn green LED ON |
| `led green off` | Turn green LED OFF |
| `led yellow on` | Turn yellow LED ON |
| `led yellow off` | Turn yellow LED OFF |
| `led red on` | Turn red LED ON |
| `led red off` | Turn red LED OFF |
| `quit` | Exit the application |
| `exit` | Exit the application |

---

### 2.3 Dual Mode

**Purpose:** Both REST API AND interactive console simultaneously

**Command:**

```bash
./wallbox_control_v4 [config] --dual
./wallbox_control_v4 [config] -d
```

**Features:**

- ✅ HTTP REST API on port 8080
- ✅ Interactive command console
- ✅ Best of both worlds
- ✅ Monitor and control from terminal while API is active

**Use Case:** Development, debugging with API, monitoring production

---

## 3. Command Combinations

### 3.1 Combination Matrix

| Config      | Interface   | Command                                 |
| ----------- | ----------- | --------------------------------------- |
| Production  | API only    | `./wallbox_control_v4 production`       |
| Production  | Interactive | `./wallbox_control_v4 production -i`    |
| Production  | Dual        | `./wallbox_control_v4 production -d`    |
| Development | API only    | `./wallbox_control_v4 dev`              |
| Development | Interactive | `./wallbox_control_v4 dev -i`           |
| Development | Dual        | `./wallbox_control_v4 dev -d`           |
| Test        | API only    | `./wallbox_control_v4 test`             |
| Test        | Interactive | `./wallbox_control_v4 test -i`          |
| Test        | Dual        | `./wallbox_control_v4 test -d`          |
| Custom      | API only    | `./wallbox_control_v4 myconfig.json`    |
| Custom      | Interactive | `./wallbox_control_v4 myconfig.json -i` |
| Custom      | Dual        | `./wallbox_control_v4 myconfig.json -d` |

### 3.2 Argument Order

Arguments can be in any order:

```bash
./wallbox_control_v4 production --dual
./wallbox_control_v4 --dual production
./wallbox_control_v4 -d prod
./wallbox_control_v4 prod -d
```

---

## 4. Running on BananaPi M5

### 4.1 Background Service (API Only)

```bash
# Start in background
ssh bananapi-m5 'cd /root/wallbox-src/build && nohup ./bin/wallbox_control_v4 production </dev/null > /tmp/wallbox.log 2>&1 &'

# Check logs
ssh bananapi-m5 'tail -f /tmp/wallbox.log'

# Stop service
ssh bananapi-m5 'pkill -f wallbox_control'
```

### 4.2 Interactive Session

```bash
# Connect with terminal allocation (-t required for interactive)
ssh -t bananapi-m5 'cd /root/wallbox-src/build && ./bin/wallbox_control_v4 production -i'
```

### 4.3 Dual Mode Session

```bash
# Connect with terminal allocation
ssh -t bananapi-m5 'cd /root/wallbox-src/build && ./bin/wallbox_control_v4 production -d'
```

### 4.4 Test from Remote (API)

```bash
# Check status via REST API
curl -s http://192.168.178.33:8080/api/status | jq .

# Enable wallbox
curl -s -X POST http://192.168.178.33:8080/api/wallbox/enable | jq .

# Disable wallbox
curl -s -X POST http://192.168.178.33:8080/api/wallbox/disable | jq .
```

---

## 5. Environment Variables

Override configuration with environment variables:

| Variable                  | Description           | Example                         |
| ------------------------- | --------------------- | ------------------------------- |
| `WALLBOX_MODE`            | Force mode (prod/dev) | `WALLBOX_MODE=prod`             |
| `WALLBOX_API_PORT`        | Override API port     | `WALLBOX_API_PORT=9000`         |
| `WALLBOX_UDP_LISTEN_PORT` | Override UDP port     | `WALLBOX_UDP_LISTEN_PORT=50020` |

**Example:**

```bash
WALLBOX_MODE=prod WALLBOX_API_PORT=9000 ./wallbox_control_v4
```

---

## 6. Quick Reference

### 6.1 Short Commands

| Short  | Long            | Description        |
| ------ | --------------- | ------------------ |
| `prod` | `production`    | Production config  |
| `dev`  | `development`   | Development config |
| `-i`   | `--interactive` | Interactive mode   |
| `-d`   | `--dual`        | Dual mode          |

### 6.2 Most Common Commands

```bash
# Production with API (most common for deployment)
./wallbox_control_v4 production

# Development with dual mode (best for development)
./wallbox_control_v4 dev -d

# Production with interactive (testing hardware)
./wallbox_control_v4 prod -i

# Production with dual (monitoring + API)
./wallbox_control_v4 prod -d
```

---

## 7. Comparison Table

| Feature              | API Mode | Interactive | Dual       |
| -------------------- | -------- | ----------- | ---------- |
| REST API             | ✅       | ❌          | ✅         |
| Console Commands     | ❌       | ✅          | ✅         |
| Background Service   | ✅       | ❌          | ❌         |
| Real-time Monitoring | ❌       | ✅          | ✅         |
| Remote Control       | ✅       | ❌          | ✅         |
| SSH Required         | No       | Yes (`-t`)  | Yes (`-t`) |

---

## 8. Troubleshooting

### 8.1 Port Already in Use

```bash
# Check what's using port 8080
netstat -tlnp | grep 8080

# Kill existing process
pkill -f wallbox_control
```

### 8.2 No Console Output

Make sure to use `ssh -t` for interactive/dual modes:

```bash
# Wrong (no terminal)
ssh bananapi-m5 './wallbox_control_v4 prod -i'

# Correct (with terminal allocation)
ssh -t bananapi-m5 './wallbox_control_v4 prod -i'
```

### 8.3 GPIO Permission Denied

```bash
# Run as root or with sudo
sudo ./wallbox_control_v4 production
```

---

## 9. Simulator Integration

### 9.1 What is the Simulator?

The **Simulator** is a separate application that simulates an electric vehicle (EV) connecting to the wallbox. It communicates via UDP to send CP (Control Pilot) state changes.

**Simulator Location:** `build/bin/simulator`

### 9.2 Simulator Compatibility

| Config Mode | Simulator Compatible | Notes                             |
| ----------- | -------------------- | --------------------------------- |
| Development | ✅ Yes               | **Recommended** - Full simulation |
| Production  | ✅ Yes               | Works alongside real hardware     |
| Test        | ✅ Yes               | For automated testing             |

### 9.3 Running with Simulator

**Terminal 1 - Start Wallbox Controller:**

```bash
./wallbox_control_v4 dev -d
```

**Terminal 2 - Start Simulator:**

```bash
./simulator
```

### 9.4 Simulator UDP Communication

| Port  | Direction           | Purpose                       |
| ----- | ------------------- | ----------------------------- |
| 50010 | Wallbox ← Simulator | Wallbox listens for CP states |
| 50011 | Wallbox → Simulator | Wallbox sends status updates  |

### 9.5 Simulator CP States

The simulator can send these CP states to simulate EV behavior:

| State | Description                       | LED Response           |
| ----- | --------------------------------- | ---------------------- |
| A     | No vehicle connected              | Yellow ON, Green OFF   |
| B     | Vehicle connected, not ready      | Yellow ON, Green BLINK |
| C     | Vehicle ready, charging requested | Yellow ON, Green BLINK |
| D     | Charging active                   | Yellow ON, Green ON    |
| E     | Error                             | Red ON                 |
| F     | Not available                     | Red ON                 |

---

## 10. External Program Integration

### 10.1 Compatibility Matrix

| External Program      | API Mode | Interactive | Dual | Development | Production |
| --------------------- | -------- | ----------- | ---- | ----------- | ---------- |
| **React App**         | ✅       | ❌          | ✅   | ✅          | ✅         |
| **Simulator**         | ✅       | ✅          | ✅   | ✅          | ✅         |
| **curl/REST clients** | ✅       | ❌          | ✅   | ✅          | ✅         |
| **Web Browser**       | ✅       | ❌          | ✅   | ✅          | ✅         |
| **Postman**           | ✅       | ❌          | ✅   | ✅          | ✅         |
| **Custom Scripts**    | ✅       | ❌          | ✅   | ✅          | ✅         |

### 10.2 React App Integration

**Location:** `web/react-app/`

**Requirements:**

- Wallbox Controller running with REST API (API or Dual mode)
- Port 8080 accessible

**Setup:**

```bash
# Terminal 1 - Start Wallbox Controller
./wallbox_control_v4 dev

# Terminal 2 - Start React App
cd web/react-app
npm install
npm start
```

**React App URL:** `http://localhost:3000`  
**API Backend:** `http://localhost:8080`

**Features:**

- Real-time status display
- Enable/Disable wallbox
- Start/Stop charging
- Relay control
- Visual LED indicators

### 10.3 Full Development Stack

For complete development environment, run all three components:

**Terminal 1 - Wallbox Controller (Dual Mode):**

```bash
cd build
./bin/wallbox_control_v4 dev -d
```

**Terminal 2 - Simulator:**

```bash
cd build
./bin/simulator
```

**Terminal 3 - React App:**

```bash
cd web/react-app
npm start
```

**Architecture:**

```
┌─────────────────┐     UDP      ┌─────────────────┐
│    Simulator    │◄────────────►│    Wallbox      │
│  (EV Simulation)│   50010/11   │   Controller    │
└─────────────────┘              └────────┬────────┘
                                          │
                                     REST API
                                     Port 8080
                                          │
                                 ┌────────▼────────┐
                                 │    React App    │
                                 │   (Frontend)    │
                                 │   Port 3000     │
                                 └─────────────────┘
```

### 10.4 Remote Access (BananaPi)

When running on BananaPi M5 in production:

**From Development Machine:**

```bash
# React App connects to BananaPi API
# Edit web/react-app/.env or package.json proxy:
REACT_APP_API_URL=http://192.168.178.33:8080

# Or use curl
curl http://192.168.178.33:8080/api/status
```

### 10.5 API Clients

Any HTTP client can interact with the REST API:

**curl:**

```bash
curl -s http://localhost:8080/api/status | jq .
curl -s -X POST http://localhost:8080/api/wallbox/enable
```

**Python:**

```python
import requests
response = requests.get('http://localhost:8080/api/status')
print(response.json())
```

**JavaScript:**

```javascript
fetch("http://localhost:8080/api/status")
  .then((res) => res.json())
  .then((data) => console.log(data));
```

**Postman:**

- Import collection or create requests manually
- Base URL: `http://localhost:8080`

---

## 11. Recommended Configurations

### 11.1 For Development (Local Machine)

```bash
# Best setup for development
./wallbox_control_v4 dev -d    # Dual mode for console + API
./simulator                     # In another terminal
npm start                       # React app in another terminal
```

### 11.2 For Hardware Testing (BananaPi)

```bash
# Test real hardware with console
ssh -t bananapi-m5 'cd /root/wallbox-src/build && ./bin/wallbox_control_v4 prod -d'
```

### 11.3 For Production Deployment (BananaPi)

```bash
# Headless service with API only
ssh bananapi-m5 'cd /root/wallbox-src/build && nohup ./bin/wallbox_control_v4 production </dev/null > /tmp/wallbox.log 2>&1 &'
```

### 11.4 For Automated Testing

```bash
# CI/CD pipeline
./wallbox_control_v4 test &
sleep 2
curl -s http://localhost:8080/health
# Run test scripts...
pkill -f wallbox_control
```

---

**Document Status:** ✅ Complete

**Last Updated:** December 26, 2025
