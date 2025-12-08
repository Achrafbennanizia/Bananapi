# API Reference

Complete API documentation for the ISO 15118 Wallbox Control System.

## Table of Contents

- [Protocol Overview](#protocol-overview)
- [Data Structures](#data-structures)
- [Enumerations](#enumerations)
- [Message Types](#message-types)
- [Network Protocol](#network-protocol)
- [Function Reference](#function-reference)
- [Examples](#examples)

## Protocol Overview

The system uses UDP-based communication with binary message structures defined in the ISO 15118 standard.

### Protocol Stack

```
┌─────────────────────────────────┐
│   Application Layer             │
│   (Terminal Commands)           │
├─────────────────────────────────┤
│   Message Layer                 │
│   (ISO 15118 Structures)        │
├─────────────────────────────────┤
│   Transport Layer               │
│   (UDP)                         │
├─────────────────────────────────┤
│   Network Layer                 │
│   (IPv4)                        │
└─────────────────────────────────┘
```

### Communication Flow

```
Wallbox Controller                    ISO Stack Simulator
      │                                      │
      │  ◄────── stSeIsoStackState ─────────│  (100ms interval)
      │                                      │
      │  ─────── stSeIsoStackCmd ──────────►│  (100ms interval)
      │                                      │
```

## Data Structures

### Core Structures

#### `stIsoStackCmd`

Command message for ISO stack control.

```cpp
struct stIsoStackCmd {
    uint8_t msgVersion;           // Protocol version (always 0)
    enIsoStackMsgType msgType;    // Message type identifier
    uint8_t enable;               // Enable flag (0=disabled, 1=enabled)
    uint16_t currentDemand;       // Requested current in 0.1A units

    void clear();                 // Reset to default values
};
```

**Fields:**

- `msgVersion`: Protocol version number (currently 0)
- `msgType`: Should be `enIsoStackMsgType::CtrlCmd` or `enIsoStackMsgType::SeCtrlCmd`
- `enable`: Charging enable flag
  - `0` = Charging disabled
  - `1` = Charging enabled
- `currentDemand`: Maximum current in 0.1A increments
  - Example: `160` = 16.0A
  - Range: 0-6500 (0.0A - 650.0A)

**Example:**

```cpp
stIsoStackCmd cmd;
cmd.clear();
cmd.msgVersion = 0;
cmd.msgType = enIsoStackMsgType::CtrlCmd;
cmd.enable = 1;
cmd.currentDemand = 160;  // 16.0A
```

#### `stIsoStackState`

State information from ISO stack.

```cpp
struct stIsoStackState {
    uint8_t msgVersion;              // Protocol version (always 0)
    enIsoStackMsgType msgType;       // Message type identifier
    enIsoChargingState state;        // Current charging state
    enSupplyPhases supplyPhases;     // Power supply configuration
    uint16_t current;                // Available current in 0.1A
    uint16_t voltage;                // Available voltage in 0.1V

    void clear();                    // Reset to default values
    void bigEndian();                // Convert to big-endian
};
```

**Fields:**

- `msgVersion`: Protocol version (0)
- `msgType`: Should be `enIsoStackMsgType::CtrlState` or `enIsoStackMsgType::SeCtrlState`
- `state`: Current charging state (see [enIsoChargingState](#enisoChargingState))
- `supplyPhases`: Power configuration (see [enSupplyPhases](#enSupplyPhases))
- `current`: Available current in 0.1A
  - Example: `160` = 16.0A
- `voltage`: Available voltage in 0.1V
  - Example: `2300` = 230.0V

**Example:**

```cpp
stIsoStackState state;
state.clear();
state.msgVersion = 0;
state.msgType = enIsoStackMsgType::CtrlState;
state.state = enIsoChargingState::charging;
state.supplyPhases = enSupplyPhases::ac3;
state.current = 160;   // 16.0A
state.voltage = 2300;  // 230.0V
```

#### `stSeHardwareState`

Hardware state information.

```cpp
struct stSeHardwareState {
    uint8_t mainContactor;      // Main contactor state (0=open, 1=closed)
    uint8_t reserved[7];        // Reserved for future use

    void clear();               // Reset to default values
};
```

**Fields:**

- `mainContactor`: Main power contactor/relay state
  - `0` = Open (power disconnected)
  - `1` = Closed (power connected)
- `reserved`: Reserved bytes for future expansion

#### `stSeHardwareCmd`

Hardware command from ISO stack.

```cpp
struct stSeHardwareCmd {
    uint8_t mainContactor;      // Main contactor command
    uint8_t sourceEnable;       // Power source enable
    uint16_t sourceVoltage;     // Target voltage in 0.1V
    uint16_t sourceCurrent;     // Target current in 0.1A
    uint8_t reserved[10];       // Reserved

    void clear();               // Reset to default values
};
```

**Fields:**

- `mainContactor`: Desired contactor state
- `sourceEnable`: Enable power source
- `sourceVoltage`: Target output voltage (0.1V)
- `sourceCurrent`: Target output current (0.1A)

### Composite Structures

#### `stSeIsoStackCmd`

Complete command including hardware state.

```cpp
struct stSeIsoStackCmd {
    stIsoStackCmd isoStackCmd;          // ISO stack command
    stSeHardwareState seHardwareState;  // Hardware state
};
```

**Size:** 24 bytes (packed)

**Usage:**

```cpp
stSeIsoStackCmd cmd;
cmd.isoStackCmd.clear();
cmd.seHardwareState.clear();
cmd.isoStackCmd.enable = 1;
cmd.isoStackCmd.currentDemand = 160;
cmd.seHardwareState.mainContactor = 1;
```

#### `stSeIsoStackState`

Complete state including hardware commands.

```cpp
struct stSeIsoStackState {
    stIsoStackState isoStackState;   // ISO stack state
    stSeHardwareCmd seHardwareCmd;   // Hardware commands
};
```

**Size:** 40 bytes (packed)

**Usage:**

```cpp
stSeIsoStackState state;
state.isoStackState.clear();
state.seHardwareCmd.clear();
state.isoStackState.state = enIsoChargingState::charging;
state.seHardwareCmd.mainContactor = 1;
state.seHardwareCmd.sourceVoltage = 2300;
state.seHardwareCmd.sourceCurrent = 160;
```

## Enumerations

### `enIsoStackMsgType`

Message type identification.

```cpp
enum class enIsoStackMsgType : uint8_t {
    CtrlCmd = 0,      // Process data commanding the stack
    CtrlState = 1,    // Process data delivering state
    ParamRead = 2,    // Read configuration parameters
    ParamWrite = 3,   // Write operational parameters
    SeCtrlCmd = 4,    // Command including SE hardware state
    SeCtrlState = 5   // State including SE hardware command
};
```

**Values:**

- `CtrlCmd` (0): Basic control command
- `CtrlState` (1): Basic state information
- `ParamRead` (2): Parameter read request (not implemented)
- `ParamWrite` (3): Parameter write request (not implemented)
- `SeCtrlCmd` (4): Supply equipment control command
- `SeCtrlState` (5): Supply equipment state response

### `enIsoChargingState`

ISO 15118 charging states.

```cpp
enum class enIsoChargingState : uint8_t {
    off = 0,            // System powered down or severe error
    idle = 1,           // No vehicle connected
    connected = 2,      // ISO message exchange in progress
    identification = 3, // Awaiting user identification
    ready = 4,          // Session established, ready to charge
    charging = 5,       // Active power transfer
    stop = 6,           // Power transfer interrupted
    finished = 7,       // Session complete, plug connected
    error = 8           // Recoverable error state
};
```

**State Diagram:**

```
    ┌─────┐
    │ off │
    └──┬──┘
       │
       ▼
    ┌──────┐     ┌───────────┐     ┌────────────────┐
    │ idle │────►│ connected │────►│ identification │
    └──────┘     └───────────┘     └────────┬───────┘
       ▲                                     │
       │                                     ▼
       │         ┌──────────┐           ┌───────┐
       │         │ finished │◄──────────│ ready │
       │         └──────────┘           └───┬───┘
       │              ▲                     │
       │              │      ┌──────┐      │
       └──────────────┼──────│ stop │◄─────┤
                      │      └──────┘      │
                      │          ▲         │
                      │          │         ▼
                      │      ┌──────────┐
                      └──────│ charging │
                             └──────────┘
```

**Helper Function:**

```cpp
std::string enIsoChargingState_toString(enIsoChargingState value);
```

### `enSupplyPhases`

Power supply configuration.

```cpp
enum class enSupplyPhases : uint8_t {
    dc = 0,   // Direct current supply
    ac1 = 1,  // Single-phase AC (230V)
    ac3 = 3   // Three-phase AC (400V)
};
```

**Power Calculation:**

```cpp
// Single-phase (ac1)
P = V × I
// Example: 230V × 16A = 3,680W = 3.68kW

// Three-phase (ac3)
P = √3 × V × I
// Example: √3 × 230V × 16A = 6,374W = 6.37kW

// DC
P = V × I
// Example: 400V × 125A = 50,000W = 50kW
```

## Network Protocol

### UDP Configuration

```cpp
// Port assignments
const int UDP_IN_PORT_WALLBOX = 50010;    // Wallbox receives
const int UDP_OUT_PORT_WALLBOX = 50011;   // Wallbox sends
const int UDP_IN_PORT_SIMULATOR = 50011;  // Simulator receives
const int UDP_OUT_PORT_SIMULATOR = 50010; // Simulator sends

// IP addresses
const char* LOCALHOST = "127.0.0.1";      // Local testing
const char* REMOTE_IP = "192.168.1.100";  // Remote deployment
```

### Message Transmission

#### Send Command (Wallbox → Simulator)

```cpp
stSeIsoStackCmd cmd;
cmd.isoStackCmd.clear();
cmd.seHardwareState.clear();

// Configure command
cmd.isoStackCmd.msgVersion = 0;
cmd.isoStackCmd.msgType = enIsoStackMsgType::SeCtrlCmd;
cmd.isoStackCmd.enable = 1;
cmd.isoStackCmd.currentDemand = 160;

// Send via UDP
sockaddr_in dest;
dest.sin_family = AF_INET;
dest.sin_port = htons(50011);
inet_pton(AF_INET, "127.0.0.1", &dest.sin_addr);

sendto(sockOut, &cmd, sizeof(cmd), 0,
       (sockaddr*)&dest, sizeof(dest));
```

#### Receive State (Wallbox ← Simulator)

```cpp
uint8_t buffer[256];
sockaddr_in src;
socklen_t srcLen = sizeof(src);

ssize_t n = recvfrom(sockIn, buffer, sizeof(buffer), 0,
                     (sockaddr*)&src, &srcLen);

if (n >= sizeof(stSeIsoStackState)) {
    stSeIsoStackState state;
    memcpy(&state, buffer, sizeof(state));

    // Process state
    if (state.isoStackState.msgType == enIsoStackMsgType::SeCtrlState) {
        // Handle state update
    }
}
```

### Timing Requirements

| Parameter        | Value  | Notes                 |
| ---------------- | ------ | --------------------- |
| Update Rate      | 100ms  | Both directions       |
| Watchdog Timeout | 2000ms | Safety feature        |
| Message Latency  | < 10ms | Typical local network |
| Jitter Tolerance | ± 50ms | Acceptable variation  |

## Function Reference

### Message Initialization

```cpp
void stIsoStackCmd::clear()
```

Reset command structure to default values.

```cpp
void stIsoStackState::clear()
```

Reset state structure to default values.

```cpp
void stSeHardwareState::clear()
```

Reset hardware state to default values.

```cpp
void stSeHardwareCmd::clear()
```

Reset hardware command to default values.

### Endianness Conversion

```cpp
void stIsoStackState::bigEndian()
```

Convert multi-byte fields to big-endian format for network transmission.

**Example:**

```cpp
stIsoStackState state;
state.current = 160;   // Native endian
state.voltage = 2300;
state.bigEndian();     // Convert to big-endian
// Now ready for network transmission
```

### Helper Functions

```cpp
std::string enIsoChargingState_toString(enIsoChargingState value)
```

Convert charging state enum to human-readable string.

**Example:**

```cpp
enIsoChargingState state = enIsoChargingState::charging;
std::string str = enIsoChargingState_toString(state);
std::cout << "State: " << str << "\n";  // Output: "State: charging"
```

## Examples

### Example 1: Basic Charging Sequence

```cpp
// Wallbox controller enables charging
stSeIsoStackCmd cmd;
cmd.isoStackCmd.clear();
cmd.isoStackCmd.msgVersion = 0;
cmd.isoStackCmd.msgType = enIsoStackMsgType::SeCtrlCmd;
cmd.isoStackCmd.enable = 1;
cmd.isoStackCmd.currentDemand = 160;  // 16.0A

sendto(sockOut, &cmd, sizeof(cmd), 0, ...);

// Simulator responds with ready state
stSeIsoStackState state;
state.isoStackState.clear();
state.isoStackState.msgVersion = 0;
state.isoStackState.msgType = enIsoStackMsgType::SeCtrlState;
state.isoStackState.state = enIsoChargingState::ready;
state.isoStackState.current = 160;
state.isoStackState.voltage = 2300;

sendto(sockOut, &state, sizeof(state), 0, ...);
```

### Example 2: Power Calculation

```cpp
// Receive state
stSeIsoStackState state;
recvfrom(sockIn, &state, sizeof(state), 0, ...);

// Calculate power
float voltage = state.isoStackState.voltage / 10.0;  // Convert to volts
float current = state.isoStackState.current / 10.0;  // Convert to amps

float power_kw;
if (state.isoStackState.supplyPhases == enSupplyPhases::ac3) {
    // Three-phase
    power_kw = (sqrt(3) * voltage * current) / 1000.0;
} else if (state.isoStackState.supplyPhases == enSupplyPhases::ac1) {
    // Single-phase
    power_kw = (voltage * current) / 1000.0;
} else {
    // DC
    power_kw = (voltage * current) / 1000.0;
}

std::cout << "Power: " << power_kw << " kW\n";
```

### Example 3: Error Handling

```cpp
// Check message validity
if (n < sizeof(stSeIsoStackState)) {
    std::cerr << "Invalid message size\n";
    return;
}

stSeIsoStackState state;
memcpy(&state, buffer, sizeof(state));

// Validate message type
if (state.isoStackState.msgType != enIsoStackMsgType::SeCtrlState &&
    state.isoStackState.msgType != enIsoStackMsgType::CtrlState) {
    std::cerr << "Invalid message type\n";
    return;
}

// Check for error state
if (state.isoStackState.state == enIsoChargingState::error) {
    std::cerr << "Charging error detected!\n";
    // Handle error condition
}

// Validate current range
if (state.isoStackState.current > 6500) {
    std::cerr << "Current out of range\n";
    return;
}
```

### Example 4: Watchdog Implementation

```cpp
auto lastReceived = std::chrono::steady_clock::now();
const auto WATCHDOG_TIMEOUT = std::chrono::milliseconds(2000);

while (running) {
    // Try to receive state
    ssize_t n = recvfrom(sockIn, buffer, sizeof(buffer), 0, ...);

    if (n > 0) {
        lastReceived = std::chrono::steady_clock::now();
        // Process message
    }

    // Check watchdog
    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - lastReceived;

    if (elapsed > WATCHDOG_TIMEOUT) {
        std::cerr << "Watchdog timeout - no communication!\n";
        // Disable charging for safety
        setElectricity(false);
    }
}
```

## Version History

| Version | Date       | Changes                          |
| ------- | ---------- | -------------------------------- |
| 0       | 2025-04-10 | Initial protocol definition      |
| 0       | 2025-11-25 | Implementation in wallbox system |
| 0       | 2025-12-08 | Documentation completed          |

## See Also

- [Main README](README.md)
- [Installation Guide](INSTALLATION.md)
- [User Guide](WallboxCtrl/README.md)
- [ISO 15118 Standard](https://www.iso.org/standard/55366.html)
