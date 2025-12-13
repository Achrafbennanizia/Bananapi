# Complete System Setup - Quick Start Guide

## 🎉 System is Running!

Your complete wallbox system is now operational with all three components:

### System Components

```
┌─────────────────────────────────────────────────────────────────┐
│                    Complete Wallbox System                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  🌐 React Web App (Mac)          🔧 Simulator (Mac)            │
│  http://localhost:3000            UDP Port 50011                │
│           │                              │                       │
│           └────────── HTTP API ──────────┘                      │
│                          │                                       │
│                          ↓                                       │
│              🖥️  Banana Pi (192.168.178.34)                    │
│                 Wallbox Control API                             │
│                 Port 8080 (HTTP)                                │
│                 Port 50010 (UDP)                                │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Access Points

### 🌐 React Web Interface

**URL:** http://localhost:3000  
**API Target:** http://192.168.178.34:8080  
**Features:**

- Real-time status monitoring
- Start/Stop charging controls
- Pause/Resume functionality
- Visual state indicators

### 🔧 Simulator (Mac)

**Location:** `/Users/achraf/pro/PJMT/WallboxCtrl/build/simulator`  
**Ports:** Listen 50011, Send to 192.168.178.34:50010  
**Log:** `/tmp/wallbox_simulator.log`

**Commands:**

```bash
# View simulator output
tail -f /tmp/wallbox_simulator.log

# To interact with simulator (if running in foreground):
> setudp 192.168.178.34 50011 50010  # Configure for Banana Pi
> status                              # Check status
> on                                  # Turn contactor on
> charge                              # Start charging
> stop                                # Stop charging
```

### 🖥️ Wallbox API (Banana Pi)

**IP:** 192.168.178.34  
**API Port:** 8080  
**UDP Port:** 50010  
**SSH:** `ssh bananapi`  
**Logs:** `/tmp/wallbox_api.out`

## Management Scripts

### Start Complete System

```bash
./start-complete-system.sh
```

Starts all three components in the correct order with health checks.

### Check System Status

```bash
./check-system-status.sh
```

Shows real-time status of all components with visual indicators.

### Stop All Components

```bash
./stop-all.sh
```

Cleanly stops wallbox, simulator, and React app.

### Test SSH Connection

```bash
./test-ssh-connection.sh
```

Tests passwordless SSH connection to Banana Pi.

## API Endpoints

### GET /api/status

Get current wallbox status

```bash
curl http://192.168.178.34:8080/api/status
```

Response:

```json
{
  "state": "IDLE",
  "wallboxEnabled": true,
  "relayEnabled": false,
  "charging": false,
  "timestamp": 1765407780
}
```

### POST /api/charging/start

Start charging session

```bash
curl -X POST http://192.168.178.34:8080/api/charging/start
```

### POST /api/charging/stop

Stop charging session

```bash
curl -X POST http://192.168.178.34:8080/api/charging/stop
```

### POST /api/charging/pause

Pause charging

```bash
curl -X POST http://192.168.178.34:8080/api/charging/pause
```

### POST /api/charging/resume

Resume charging

```bash
curl -X POST http://192.168.178.34:8080/api/charging/resume
```

### GET /health

Health check endpoint

```bash
curl http://192.168.178.34:8080/health
```

## Quick Commands

### Mac (Local)

```bash
# Check all processes
./check-system-status.sh

# View React logs
tail -f /tmp/react_app.log

# View Simulator logs
tail -f /tmp/wallbox_simulator.log

# Test API
curl http://192.168.178.34:8080/api/status | python3 -m json.tool

# Restart React only
lsof -ti:3000 | xargs kill -9
cd wallbox-react-app && BROWSER=none npm start &
```

### Banana Pi (Remote)

```bash
# SSH to Banana Pi
ssh bananapi

# Check wallbox process
ssh bananapi 'pgrep -f wallbox_control_api'

# View logs
ssh bananapi 'tail -f /tmp/wallbox_api.out'

# Restart wallbox
ssh bananapi 'killall wallbox_control_api; cd ~/wallbox-src/build && nohup ./wallbox_control_api > /tmp/wallbox_api.out 2>&1 &'

# Check API locally
ssh bananapi 'curl -s http://localhost:8080/api/status'
```

## Troubleshooting

### React App Not Loading

```bash
# Check if running
lsof -ti:3000

# Check logs
tail -f /tmp/react_app.log

# Restart
lsof -ti:3000 | xargs kill -9
cd /Users/achraf/pro/PJMT/wallbox-react-app
BROWSER=none npm start > /tmp/react_app.log 2>&1 &
```

### API Not Responding

```bash
# Check process on Banana Pi
ssh bananapi 'pgrep -f wallbox_control_api'

# Check logs
ssh bananapi 'tail -30 /tmp/wallbox_api.out'

# Restart
ssh bananapi 'killall wallbox_control_api; cd ~/wallbox-src/build && nohup ./wallbox_control_api > /tmp/wallbox_api.out 2>&1 &'
```

### Simulator Issues

```bash
# Check if running
pgrep -f simulator

# Check port
lsof -i:50011

# Restart
pkill -f simulator
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./simulator > /tmp/wallbox_simulator.log 2>&1 &
```

### Network Issues

```bash
# Test Banana Pi connectivity
ping -c 3 192.168.178.34

# Test API connectivity
curl -v --max-time 3 http://192.168.178.34:8080/api/status

# Check your Mac's IP
ipconfig getifaddr en0
```

## Development Workflow

### Making Changes to React App

```bash
cd /Users/achraf/pro/PJMT/wallbox-react-app

# Edit files in src/
# Changes auto-reload in browser

# Check console for errors
tail -f /tmp/react_app.log
```

### Making Changes to Wallbox Code

```bash
# Edit locally
cd /Users/achraf/pro/PJMT/WallboxCtrl/src

# Sync to Banana Pi
rsync -avz /Users/achraf/pro/PJMT/WallboxCtrl/ bananapi:~/wallbox-src/

# Rebuild on Banana Pi
ssh bananapi 'cd ~/wallbox-src/build && make wallbox_control_api'

# Restart
ssh bananapi 'killall wallbox_control_api; cd ~/wallbox-src/build && nohup ./wallbox_control_api > /tmp/wallbox_api.out 2>&1 &'
```

### Making Changes to Simulator

```bash
# Edit simulator
cd /Users/achraf/pro/PJMT/WallboxCtrl/src

# Rebuild
cd ../build && make simulator

# Restart
pkill -f simulator
./simulator > /tmp/wallbox_simulator.log 2>&1 &
```

## Configuration Files

### React App Environment

**File:** `/Users/achraf/pro/PJMT/wallbox-react-app/.env`

```env
REACT_APP_API_BASE_URL=http://192.168.178.34:8080
```

### Wallbox Configuration

**File:** `~/wallbox-src/build/config.json` (on Banana Pi)

```json
{
  "mode": "development",
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "192.168.178.23",
    "api_port": 8080
  }
}
```

### SSH Configuration

**File:** `~/.ssh/config`

```ssh-config
Host bananapi
    HostName 192.168.178.34
    User root
    IdentityFile ~/.ssh/bananapi_key
```

## System Architecture

### Data Flow

1. **User** → React Web App (localhost:3000)
2. **React App** → HTTP API (192.168.178.34:8080)
3. **Wallbox API** ↔ Simulator (UDP 50010/50011)
4. **Wallbox** → GPIO Control (Simulated in dev mode)

### State Management

- **IDLE**: Ready, no charging
- **READY**: Vehicle connected, ready to charge
- **CHARGING**: Active charging session
- **PAUSED**: Charging paused
- **STOPPED**: Charging stopped
- **ERROR**: Error state

## Next Steps

1. **Open Web Interface**

   ```bash
   open http://localhost:3000
   ```

2. **Test Charging Cycle**

   - Click "Start Charging" in web interface
   - Watch state change to CHARGING
   - Check simulator logs for UDP messages
   - Click "Stop Charging"

3. **Monitor System**

   ```bash
   # Watch all logs
   ./check-system-status.sh

   # Or individual components
   tail -f /tmp/react_app.log
   tail -f /tmp/wallbox_simulator.log
   ssh bananapi 'tail -f /tmp/wallbox_api.out'
   ```

## Useful Links

- **React App:** http://localhost:3000
- **API Status:** http://192.168.178.34:8080/api/status
- **API Health:** http://192.168.178.34:8080/health
- **Banana Pi SSH:** `ssh bananapi`

## Support Scripts Location

All management scripts are in `/Users/achraf/pro/PJMT/`:

- `start-complete-system.sh` - Start everything
- `check-system-status.sh` - Check status
- `stop-all.sh` - Stop everything
- `test-ssh-connection.sh` - Test SSH

---

**System Status:** ✅ All components operational  
**Last Updated:** December 10, 2024  
**Version:** Complete System v1.0
