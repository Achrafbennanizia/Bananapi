# Wallbox Controller v4.0

## Overview

Version 4.0 is a **simplified** version that focuses solely on relay control. All complex functionality (UDP communication, state machine, HTTP API) has been removed for maximum simplicity.

## Key Features

- ✅ **Simplified Architecture** - Only relay control, no networking
- ✅ **C++14 Standard** - Modern C++ without experimental features
- ✅ **GPIO Pin 21** - Default relay pin changed from 4 to 21
- ✅ **No Dependencies** - No UDP, no HTTP server, no state machine
- ✅ **Direct Control** - Simple on/off relay commands

## What's Different from v3?

| Feature           | v3  | v4  |
| ----------------- | --- | --- |
| UDP Communication | ✅  | ❌  |
| HTTP REST API     | ✅  | ❌  |
| State Machine     | ✅  | ❌  |
| Network Messages  | ✅  | ❌  |
| Relay Control     | ✅  | ✅  |
| Default Pin       | 4   | 21  |
| C++ Standard      | 17  | 14  |

## Building

```bash
cd WallboxCtrl/build
cmake ..
make wallbox_control_v4
```

## Running

### Development Mode (Simulator)

```bash
export WALLBOX_MODE=dev
./wallbox_control_v4
```

### Production Mode (Real Hardware)

```bash
export WALLBOX_MODE=prod
./wallbox_control_v4
```

## Commands

Once running, use these simple commands:

- `on` - Enable relay (turn on)
- `off` - Disable relay (turn off)
- `status` - Show current relay status
- `quit` or `exit` - Exit the program

## Example Session

```
==================================================
  Wallbox Controller v4.0 - Simple Relay Control
==================================================
Mode: development
GPIO Type: stub
Relay Pin: 21

=== Wallbox Controller Ready ===
Commands:
  on    - Enable relay
  off   - Disable relay
  status - Show relay status
  quit   - Exit
================================

> on
Relay ENABLED on pin 21

> status
Relay status: ON

> off
Relay DISABLED on pin 21

> quit
Goodbye!
```

## Architecture

### Components

1. **SimpleWallboxController** - Minimal controller with only relay control
2. **IGpioController** - GPIO interface (stub or real hardware)
3. **GpioFactory** - Creates appropriate GPIO controller based on mode
4. **Configuration** - Basic configuration management

### No Longer Included

- ❌ UdpCommunicator
- ❌ HttpApiServer
- ❌ ChargingStateMachine
- ❌ NetworkCommunicator
- ❌ ApiController
- ❌ Application class

## Configuration

Default relay pin is **GPIO 21**. This is set in the code and can be changed in `Configuration.h`:

```cpp
m_relayPin(21)  // v4.0 default: GPIO 21
```

## Use Case

Use v4 when you need:

- Simple relay control only
- No network communication
- Minimal dependencies
- Quick testing
- Learning the basics

For full features (REST API, state machine, UDP), use **v3**.
