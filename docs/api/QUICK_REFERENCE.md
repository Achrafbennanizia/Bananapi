# Quick Reference Guide

## Installation (One-Liner)

```bash
git clone https://github.com/Achrafbennanizia/Bananapi.git && cd Bananapi/WallboxCtrl/src && g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl && g++ -std=c++17 -O2 simulator.cpp IsoStackCtrlProtocol_impl.cpp -o simulator
```

## Quick Start

**Terminal 1:**

```bash
cd Bananapi/WallboxCtrl/src && ./wallbox_ctrl
```

**Terminal 2:**

```bash
cd Bananapi/WallboxCtrl/src && ./simulator
```

## Essential Commands

### Wallbox Controller

| Command   | Action           |
| --------- | ---------------- |
| `enable`  | Enable charging  |
| `disable` | Disable charging |
| `status`  | Show status      |
| `quit`    | Exit program     |

### Simulator

| Command  | Action             |
| -------- | ------------------ |
| `on`     | Turn contactor ON  |
| `off`    | Turn contactor OFF |
| `ready`  | Set READY state    |
| `charge` | Start CHARGING     |
| `stop`   | Stop charging      |
| `status` | Show status        |

## Common Workflows

### Start Charging Session

```
Wallbox: enable
Simulator: on
Simulator: ready
Simulator: charge
```

### Stop Charging Session

```
Simulator: stop
Simulator: off
Wallbox: disable
```

### Check Status

```
Wallbox: status
Simulator: status
```

## Network Configuration

| Component | Port  | Direction |
| --------- | ----- | --------- |
| Wallbox   | 50010 | Receive   |
| Wallbox   | 50011 | Send      |
| Simulator | 50011 | Receive   |
| Simulator | 50010 | Send      |

## Message Structures

### Command (24 bytes)

```
msgVersion: 0
msgType: SeCtrlCmd (4)
enable: 0/1
currentDemand: 0-6500 (×0.1A)
```

### State (40 bytes)

```
msgVersion: 0
msgType: SeCtrlState (5)
state: idle/ready/charging/...
current: 0-6500 (×0.1A)
voltage: 0-65535 (×0.1V)
```

## Charging States

```
off → idle → connected → identification → ready ⇄ charging → stop → finished
                                             ↓      ↓          ↓
                                           error ←--+----------+
```

## Troubleshooting

**Port in use:**

```bash
lsof -ti:50010 | xargs kill -9
lsof -ti:50011 | xargs kill -9
```

**Build error:**

```bash
g++ --version  # Check C++17 support
```

**No communication:**

```bash
sudo ufw allow 50010/udp
sudo ufw allow 50011/udp
```

**GPIO error (expected on non-Pi):**

```
[HAL] GPIO system initialized (stub)
# This is normal for development systems
```

## File Locations

```
Bananapi/
├── README.md              # Main documentation
├── INSTALLATION_GUIDE.md        # Setup guide
├── API_REFERENCE.md       # Protocol details
├── DEVELOPMENT.md         # Dev guide
├── CHANGELOG.md           # Version history
└── WallboxCtrl/
    ├── src/
    │   ├── wallbox_ctrl   # Main executable
    │   └── simulator      # Simulator executable
    └── README.md          # Component docs
```

## Build Commands

**Simple:**

```bash
g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl
```

**Debug:**

```bash
g++ -std=c++17 -g main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl
```

**CMake:**

```bash
cd build && cmake .. && make
```

## GPIO Pin (Raspberry Pi)

| Pin    | Function      |
| ------ | ------------- |
| GPIO 5 | Relay control |
| GND    | Ground        |
| 5V     | Power         |

## Typical Values

| Parameter           | Value       |
| ------------------- | ----------- |
| Voltage (AC single) | 230V (2300) |
| Voltage (AC three)  | 230V (2300) |
| Current             | 16A (160)   |
| Power (AC1)         | 3.68 kW     |
| Power (AC3)         | 6.37 kW     |
| Update rate         | 100ms       |
| Watchdog            | 2000ms      |

## Key Shortcuts

**Graceful shutdown:**

- Press `Ctrl+C` in either terminal

**Force quit:**

- Type `quit` or `exit` in terminal prompt

## Getting Help

📖 Full docs: `README.md`
🔧 Installation: `INSTALLATION_GUIDE.md`
📡 API: `API_REFERENCE.md`
👨‍💻 Development: `DEVELOPMENT.md`
📋 History: `CHANGELOG.md`

🐛 Issues: https://github.com/Achrafbennanizia/Bananapi/issues
💬 Discussions: https://github.com/Achrafbennanizia/Bananapi/discussions

## Version

Current: **1.0.0** (2025-12-08)
Latest: Check [Releases](https://github.com/Achrafbennanizia/Bananapi/releases)

---

_For detailed information, see the complete documentation files._
