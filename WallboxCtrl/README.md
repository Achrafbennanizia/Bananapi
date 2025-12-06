# Wallbox Control System

This system consists of two communicating programs that simulate an ISO 15118 charging control system.

## Components

### 1. **wallbox_ctrl** - Wallbox Controller

- Controls the main relay/contactor based on commands from the ISO stack
- Listens on UDP port **50010**
- Sends status to UDP port **50011**
- Implements watchdog timeout (2 seconds) for safety

### 2. **simulator** - ISO 15118 Stack Simulator

- Simulates the ISO 15118 charging stack
- Sends commands to the wallbox controller
- Listens on UDP port **50011**
- Sends state to UDP port **50010**
- Interactive command-line interface

## Building

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/src

# Build both programs
g++ -std=c++17 -O2 main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl
g++ -std=c++17 -O2 simulator.cpp IsoStackCtrlProtocol_impl.cpp -o simulator
```

## Running

### Terminal 1: Start the Wallbox Controller

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/src
./wallbox_ctrl
```

### Terminal 2: Start the Simulator

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/src
./simulator
```

## Simulator Commands

Once the simulator is running, you can use these interactive commands:

| Command  | Description                           |
| -------- | ------------------------------------- |
| `on`     | Turn main contactor ON (relay closes) |
| `off`    | Turn main contactor OFF (relay opens) |
| `idle`   | Set charging state to IDLE            |
| `ready`  | Set charging state to READY           |
| `charge` | Set charging state to CHARGING        |
| `stop`   | Set charging state to STOP            |
| `status` | Show current simulator status         |
| `help`   | Show available commands               |
| `quit`   | Exit simulator                        |

## Testing Sequence

1. Start both programs in separate terminals
2. In the simulator, type: `on`
   - You should see the wallbox_ctrl output: `[RX] mainContactor=true` and `[GPIO] Relay ON`
3. Type: `off`
   - You should see: `[RX] mainContactor=false` and `[GPIO] Relay OFF`
4. Try changing states: `ready`, `charge`, etc.

## Safety Features

- **Watchdog Timer**: If wallbox_ctrl doesn't receive valid packets for 2 seconds, the relay automatically turns OFF
- **Graceful Shutdown**: Both programs respond to Ctrl+C for clean shutdown
- **GPIO Stub**: The GPIO functions are stubbed for non-Raspberry Pi systems (prints to console instead)

## Communication Protocol

Both programs use the ISO 15118 control protocol defined in `IsoStackCtrlProtocol.h`:

- **stSeIsoStackState** (48 bytes): Sent by simulator → Received by wallbox_ctrl

  - Contains `mainContactor` command (byte offset 40)
  - Contains charging state and voltage/current info

- **stSeIsoStackCmd** (16 bytes): Sent by wallbox_ctrl → Received by simulator
  - Contains enable flag and current demand

## Network Configuration

All communication uses UDP on localhost (127.0.0.1). To use over network:

1. Edit `SIM_IP` in `main.cpp` (wallbox_ctrl)
2. Edit `WALLBOX_IP` in `simulator.cpp`
3. Rebuild both programs
