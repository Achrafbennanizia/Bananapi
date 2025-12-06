# IsoStackCtrlProtocol Library

ISO 15118 Stack Control Protocol implementation for wallbox/charging station control.

## Overview

This library provides message protocol structures for controlling ISO 15118 charging stacks, including:

- Command and state messages for stack control
- Hardware control messages for supply equipment
- Big-endian byte order conversion utilities

## Building

### Using Make (Recommended)

```bash
# Build the library
make

# Build and run tests
make test

# Clean build artifacts
make clean

# Show help
make help
```

### Manual Compilation

```bash
# Compile object file
g++ -std=c++17 -c IsoStackCtrlProtocol.cpp -o IsoStackCtrlProtocol.o

# Create static library
ar rcs libIsoStackCtrl.a IsoStackCtrlProtocol.o

# Link with your program
g++ -std=c++17 your_program.cpp -L. -lIsoStackCtrl -o your_program
```

## Files

- `IsoStackCtrlProtocol.h` - Header with protocol definitions
- `IsoStackCtrlProtocol.cpp` - Implementation
- `libIsoStackCtrl.a` - Compiled static library (after build)
- `Makefile` - Build automation
- `test_iso.cpp` - Test/example program

## Usage Example

```cpp
#include "IsoStackCtrlProtocol.h"
using namespace Iso15118;

// Create and initialize a command
stSeIsoStackCmd cmd;
cmd.isoStackCmd.enable = 1;
cmd.isoStackCmd.currentDemand = 160; // 16.0 A
cmd.seHardwareState.mainContactor = 1;

// Create and initialize a state
stSeIsoStackState state;
state.isoStackState.state = enIsoChargingState::charging;
state.isoStackState.voltage = 2300; // 230.0 V
state.seHardwareCmd.mainContactor = 1;
```

## Message Structures

### stIsoStackCmd (8 bytes)

Command message for ISO stack control

- `msgVersion`: Message format version
- `msgType`: Message type identifier
- `enable`: Enable/disable charging
- `identification`: User identification confirmation
- `currentDemand`: Demanded current (A/10)

### stIsoStackState (52 bytes)

State message from ISO stack

- `msgType`: Message type
- `state`: Charging state (enIsoChargingState)
- `current`, `voltage`: Actual values (unit/10)
- `seccId`: Supply equipment ID
- `evccId`: Vehicle ID
- Session and energy information

### stSeHardwareCtrl (10 bytes)

Hardware control for supply equipment

- `mainContactor`: Main relay control/state
- `sourceEnable`: Power source enable
- `sourceVoltage`, `sourceCurrent`: Power limits

### stSeIsoStackCmd (16 bytes)

Combined ISO stack command + hardware state

### stSeIsoStackState (48 bytes)

Combined ISO stack state + hardware command

## Charging States

- `off`: No power supply
- `idle`: No vehicle connected
- `connected`: ISO message exchange
- `identification`: Awaiting ID confirmation
- `ready`: Session setup complete
- `charging`: Power transfer active
- `stop`: Power transfer interrupted
- `finished`: Charging complete, plug connected
- `error`: Resettable error occurred

## Dependencies

- C++17 or later
- Standard library (`<algorithm>`, `<cstdint>`, `<string>`)

## Notes

- All multi-byte values use big-endian byte order
- Structures are packed (`#pragma pack(push, 1)`)
- No external dependencies (LibCpp/HAL dependency removed)

## License

Published under MIT license

## Authors

- Dr. Olaf Simon
- KEA - Science to Business GmbH - Hochschule Osnabrück
