# Quick Start Guide - Wallbox Control System v4.1

## 🚀 Quick Installation

### Option 1: Interactive Installation (Recommended)

```bash
cd /path/to/project/WallboxCtrl
./scripts/install.sh --interactive
```

### Option 2: Direct Mode Selection

```bash
# Production mode (optimized)
./scripts/install.sh --mode production

# Development mode (debug symbols)
./scripts/install.sh --mode development

# Debug mode (sanitizers)
./scripts/install.sh --mode debug
```

## 🎯 Running the System

### Terminal 1 - Wallbox Controller

```bash
cd /path/to/project/build/bin
./wallbox_control_v3
```

**Features:**

- ✅ HTTP REST API on port 8080
- ✅ UDP communication on port 50010
- ✅ Clean terminal output
- ✅ Structured logging to `/tmp/wallbox_v3.log`

### Terminal 2 - ISO 15118 Simulator

```bash
cd /path/to/project/build/bin
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

### 3. Test API (Optional)

```bash
# Check status via HTTP API
curl http://localhost:8080/api/status

# Enable wallbox
curl -X POST http://localhost:8080/api/enable

# Disable wallbox
curl -X POST http://localhost:8080/api/disable
```

## 📦 Deployment to Raspberry Pi

### Interactive Deployment

```bash
cd /path/to/project/WallboxCtrl
./scripts/deploy.sh <API_HOST> --interactive
```

### Direct Deployment

```bash
# Production mode
./scripts/deploy.sh <API_HOST> --mode production

# Development mode with custom user
PI_USER=admin ./scripts/deploy.sh <API_HOST> --mode development
```

## 📊 View Logs

### Simulator Log (All UDP Traffic)

```bash
tail -f /tmp/wallbox_simulator.log
```

### Wallbox Log

```bash
tail -f /tmp/wallbox_v3.log
```

## 🔧 Configuration

Edit `config.json` in the build directory:

```json
{
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "127.0.0.1",
    "api_port": 8080
  }
}
```

## 🆘 Troubleshooting

### Processes Won't Start

```bash
# Kill existing processes
pkill -9 wallbox_control; pkill -9 simulator

# Check ports
lsof -i :8080
lsof -i :50010
lsof -i :50011
```

### UDP Communication Issues

```bash
# Simulator: check current UDP config
> getudp

# Simulator: change to localhost
> setudp 127.0.0.1 50011 50010
```

### View Help

```bash
# Deployment help
./scripts/deploy.sh --help

# Installation help
./scripts/install.sh --help
```

## 📚 More Information

- **Full Documentation**: See `/docs` directory
- **API Reference**: `/docs/api/API_REFERENCE.md`
- **Deployment Guide**: `WallboxCtrl/scripts/README.md`
- **Architecture**: `/docs/architecture/ARCHITECTURE.md`
  > on
  > ready
  > charge
  > status
  ```

  ```

3. **Watch the interaction - charging should start!**

4. **To stop charging:**
   ```
   Simulator: > stop
   Wallbox: > disable
   ```

## Log Files

All diagnostic logs are written to files (no terminal clutter):

- **Wallbox logs:** `/tmp/wallbox_main.log`
- **Simulator logs:** `/tmp/wallbox_simulator.log`

**View logs in real-time (optional, separate terminal):**

```bash
# View wallbox logs
tail -f /tmp/wallbox_main.log

# View simulator logs
tail -f /tmp/wallbox_simulator.log
```

## Advanced Mode (With React UI)

### Option A: Full Background Mode (Web Control Only)

```bash
bash scripts/start-dev.sh
cd web/react-app && npm start
```

Control everything through the web interface.

### Option B: Interactive Dev Mode (Terminal + Web)

Run these in **separate terminals**:

**Terminal 1 - API Server:**

```bash
cd /path/to/project
bash scripts/start-api-only.sh
```

**Terminal 2 - Interactive Simulator:**

```bash
cd /path/to/project/build/bin
./simulator
```

Now you can type commands like: `on`, `ready`, `charge`, `status`

**Terminal 3 - React App:**

```bash
cd /path/to/project/wallbox-react-app
npm start
```

Open http://localhost:3000

**You now have:**

- ✅ Interactive simulator (type commands in terminal)
- ✅ React web interface (control via browser)
- ✅ Both working at the same time!

---

**Terminals are clean - just enter commands and see the program reactions!**
