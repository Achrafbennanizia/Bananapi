# Interactive Wallbox Control System

## Overview

Both programs now support **interactive terminal commands** while maintaining **UDP communication**.

## Features

✅ **Wallbox Control** - Accepts commands to enable/disable charging  
✅ **ISO Simulator** - Accepts commands to control main contactor and charging state  
✅ **UDP Communication** - Bidirectional real-time communication  
✅ **Smart Logging** - Only shows messages when state changes (no spam)  
✅ **Non-blocking Input** - Commands don't interrupt UDP communication

## Running the System

### Option 1: Two Terminals (Recommended)

**Terminal 1 - Wallbox Control:**

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/src
./wallbox_ctrl
```

**Terminal 2 - ISO Simulator:**

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/src
./simulator
```

### Option 2: Using Test Script

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl
./test_interactive.sh
```

## Available Commands

### Wallbox Control Commands

| Command   | Description                                         |
| --------- | --------------------------------------------------- |
| `enable`  | Enable charging operation (default)                 |
| `disable` | Disable charging (safety override)                  |
| `status`  | Show relay, enable, watchdog, and connection status |
| `help`    | Show command list                                   |
| `quit`    | Exit program                                        |

### Simulator Commands

| Command  | Description                                 |
| -------- | ------------------------------------------- |
| `on`     | Turn main contactor ON → Relay activates    |
| `off`    | Turn main contactor OFF → Relay deactivates |
| `idle`   | Set charging state to IDLE                  |
| `ready`  | Set charging state to READY                 |
| `charge` | Set charging state to CHARGING              |
| `stop`   | Set charging state to STOP                  |
| `status` | Show main contactor and charging state      |
| `help`   | Show command list                           |
| `quit`   | Exit program                                |

## Communication Flow

```
┌─────────────────┐          UDP          ┌──────────────────┐
│   Simulator     │ ◄─────────────────►   │ Wallbox Control  │
│                 │                       │                  │
│ Port: 50011 IN  │   stSeIsoStackState   │ Port: 50010 IN   │
│ Port: 50010 OUT │   stSeIsoStackCmd     │ Port: 50011 OUT  │
└─────────────────┘                       └──────────────────┘
       ▲                                            ▲
       │                                            │
   Terminal                                    Terminal
   Commands                                    Commands
```

## Test Sequence

1. **Start both programs** in separate terminals
2. **Verify connection** - Both should show initial state
3. **In Simulator**: Type `on`
   - Simulator sends: `mainContactor=1`
   - Wallbox receives: `[RX] mainContactor=true`
   - Wallbox activates: `[GPIO] Relay ON`
4. **In Wallbox**: Type `disable`
   - Wallbox sets: `enable=false`
   - Relay turns: `[GPIO] Relay OFF`
   - Simulator receives: `[RX] enable=false`
5. **In Wallbox**: Type `enable`
   - Relay turns back: `[GPIO] Relay ON`
6. **In Simulator**: Type `off`
   - Relay turns: `[GPIO] Relay OFF`

## State Change Detection

Both programs now use **smart logging**:

- ✅ Only log UDP messages when values change
- ✅ Reduces terminal spam
- ✅ Makes state changes clearly visible
- ✅ Maintains command prompt `>` after each message

## Safety Features

1. **Watchdog Timer** (Wallbox)
   - 2-second timeout
   - Automatically disables relay if no ISO messages received
2. **Enable Override** (Wallbox)

   - `disable` command immediately turns off relay
   - Works even if simulator requests `on`

3. **Non-blocking Operation**
   - Commands don't block UDP communication
   - Real-time response to both terminal and network

## Troubleshooting

**Problem**: Port already in use

```bash
# Kill existing processes
killall wallbox_ctrl simulator

# Or find and kill specific PIDs
lsof -i :50010
lsof -i :50011
```

**Problem**: Programs not responding to commands

- Check if program is running: `ps aux | grep wallbox`
- Verify no firewall blocking UDP ports
- Try restarting both programs

**Problem**: No UDP communication

- Ensure both programs started successfully
- Check logs: `/tmp/wallbox.log` and `/tmp/simulator.log`
- Verify ports 50010 and 50011 are available

## Building

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/src

# Build wallbox control
g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl

# Build simulator
g++ -std=c++17 -O2 simulator.cpp IsoStackCtrlProtocol_impl.cpp -o simulator
```

## Architecture

- **Language**: C++17
- **Network**: UDP sockets (non-blocking)
- **Input**: Non-blocking stdin with select()
- **Protocol**: ISO 15118 Stack Control Protocol
- **Threading**: Single-threaded event loop

## Log Files

When using test script, logs are saved to:

- `/tmp/wallbox.log` - Wallbox control messages
- `/tmp/simulator.log` - Simulator messages

## Notes

- All GPIO operations are stubbed (print to console)
- For real Raspberry Pi, replace HAL stubs with actual GPIO library
- Voltage values in protocol are in V/10 (2300 = 230.0V)
- Current values in protocol are in A/10 (160 = 16.0A)
