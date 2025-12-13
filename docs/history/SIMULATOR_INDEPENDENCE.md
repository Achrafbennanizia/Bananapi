# Simulator Independence Update

**Date**: December 10, 2025
**Status**: ✅ Implemented (Updated)

## Overview

The simulator has been modified to operate independently without tracking or synchronizing with wallbox status. The simulator now only manages its own state and sends commands to the wallbox, while the wallbox responds according to its own state machine.

**Latest Update**: The simulator now displays wallbox feedback messages (enable/disable, relay state changes) without storing these as persistent state. This provides visibility into wallbox responses while maintaining simulator independence.

## Changes Made

### 1. Removed Variables

The following wallbox status tracking variables have been completely removed:

```cpp
static bool g_lastEnableState = true;     // ❌ REMOVED
static uint16_t g_lastCurrentDemand = 0;  // ❌ REMOVED
static bool g_lastRelayState = false;     // ❌ REMOVED
static bool g_firstMessage = true;        // ❌ REMOVED
```

### 2. Remaining Variables (Simulator's Own State)

The simulator maintains only its own state:

```cpp
static bool g_run = true;                           // Program running flag
static bool g_mainContactorCmd = false;             // Contactor command to send
static enIsoChargingState g_chargingState = idle;   // Simulator's charging state
static enIsoChargingState g_prevChargingState = idle; // For change detection
static bool g_enableCommand = true;                 // Enable/disable command to send
```

### 3. Enhanced `recv_cmd()` Function

The function now:

- Receives UDP messages from wallbox
- Validates message type
- **Displays wallbox feedback messages** when enable/relay state changes
- Uses local static variables for change detection only (not global state)
- Logs reception periodically (every 100 messages)
- **Does NOT** sync simulator state with wallbox

```cpp
void recv_cmd(int sock)
{
    // ... receive and validate message ...

    // Display wallbox feedback without storing state
    static bool prevEnableState = true;
    static bool prevRelayState = false;

    bool wallboxEnable = (cmd.isoStackCmd.enable != 0);
    bool wallboxRelay = cmd.seHardwareState.mainContactor;

    // Show error/status messages when wallbox state changes
    if (wallboxEnable != prevEnableState)
    {
        if (!wallboxEnable)
        {
            std::cout << "[WALLBOX FEEDBACK] 🔴 Wallbox is now DISABLED - Commands may be rejected" << std::endl;
        }
        else
        {
            std::cout << "[WALLBOX FEEDBACK] 🟢 Wallbox is now ENABLED - Ready to accept commands" << std::endl;
        }
        prevEnableState = wallboxEnable;
    }

    if (wallboxRelay != prevRelayState)
    {
        std::cout << "[WALLBOX FEEDBACK] " << (wallboxRelay ? "⚡ Main contactor activated" : "🔌 Main contactor deactivated") << std::endl;
        prevRelayState = wallboxRelay;
    }
}
```

**Key Points:**

- `prevEnableState` and `prevRelayState` are **local static** variables (function scope only)
- Used only for change detection to avoid spamming the console
- **Not accessible** from other functions
- **Not used** for command validation or state synchronization

### 4. Removed Functions

- `print_wallbox_status()` - ❌ Removed (was used by `getwb` command)
- `ensure_wallbox_enabled()` - ❌ Removed (validation based on wallbox state)

### 5. Removed Commands

- `getwb` / `wallbox` - ❌ Removed from command list and help

### 6. Simplified Command Processing

All state change commands (`on`, `off`, `idle`, `ready`, `charge`, `stop`) now:

- ✅ Execute immediately without checking wallbox status
- ✅ No validation against wallbox relay/enable state
- ✅ Only update simulator's own state variables
- ✅ Wallbox feedback is displayed passively if it rejects commands

**Before (with validation):**

```cpp
if (cmd == "charge")
{
    if (!ensure_wallbox_enabled("charge")) return;  // ❌ REMOVED
    if (!g_lastRelayState) return;                  // ❌ REMOVED
    g_chargingState = enIsoChargingState::charging;
    // ... logging ...
}
```

**After (independent operation):**

```cpp
if (cmd == "charge")
{
    g_chargingState = enIsoChargingState::charging;
    std::cout << "✓ Charging state changed to: CHARGING" << std::endl;
    log_msg("CMD", "State: CHARGING");
}
```

## Communication Model

### New Model: Command-Based (Independent)

```
┌──────────────┐                    ┌──────────────┐
│  SIMULATOR   │                    │   WALLBOX    │
│              │                    │              │
│  • Own State │───── Commands ────▶│  • Own State │
│  • Contactor │◀──── Status* ─────│  • Relay     │
│  • Enable    │                    │  • Enable    │
└──────────────┘                    └──────────────┘

* Wallbox status feedback is displayed but not stored
```

**Simulator sends:**

- State (idle, ready, charging, stop)
- Contactor command (on/off)
- Enable command (enable/disable)

**Wallbox responds:**

- Processes simulator commands according to its state machine
- Changes relay based on state transitions
- Sends status back with feedback (enable/disable, relay on/off)

**Simulator displays feedback:**

- 🟢 "Wallbox is now ENABLED"
- 🔴 "Wallbox is now DISABLED - Commands may be rejected"
- ⚡ "Main contactor activated"
- 🔌 "Main contactor deactivated"

## Benefits

1. **Independence**: Simulator and wallbox operate independently
2. **Visibility**: User sees wallbox feedback without state coupling
3. **Simplicity**: No complex synchronization logic
4. **Testing**: Easier to test edge cases and race conditions
5. **Clarity**: Each component manages only its own state

## Testing

### Test 1: Basic Communication

```bash
./simulator
# Simulator receives wallbox messages and displays feedback
# Every 100 messages: "DEBUG: Received 100 messages from wallbox"
```

### Test 2: State Commands

```bash
> ready    # Simulator immediately goes to READY
> charge   # Simulator immediately goes to CHARGING
> stop     # Simulator immediately goes to STOP
```

### Test 3: Enable/Disable with Feedback

```bash
> disable  # Simulator sends enable=false to wallbox
           # Wallbox responds, simulator displays:
           # [WALLBOX FEEDBACK] 🔴 Wallbox is now DISABLED - Commands may be rejected

> enable   # Simulator sends enable=true to wallbox
           # Wallbox responds, simulator displays:
           # [WALLBOX FEEDBACK] 🟢 Wallbox is now ENABLED - Ready to accept commands
```

### Test 4: Relay State Feedback

```bash
> on       # Simulator requests contactor ON
           # If wallbox accepts, displays:
           # [WALLBOX FEEDBACK] ⚡ Main contactor activated

> off      # Simulator requests contactor OFF
           # Wallbox responds:
           # [WALLBOX FEEDBACK] 🔌 Main contactor deactivated
```

## Feedback Messages

The simulator displays these feedback messages based on wallbox responses:

| Message                       | Meaning                                             |
| ----------------------------- | --------------------------------------------------- |
| 🟢 Wallbox is now ENABLED     | Wallbox accepted enable command, ready for charging |
| 🔴 Wallbox is now DISABLED    | Wallbox is disabled, may reject charging commands   |
| ⚡ Main contactor activated   | Wallbox has closed the main contactor               |
| 🔌 Main contactor deactivated | Wallbox has opened the main contactor               |

**Important**: These are **display-only messages**. The simulator does not validate commands based on this feedback.

## Available Commands

### State Commands

- `idle` - Set state to IDLE
- `ready` - Set state to READY
- `charge` - Set state to CHARGING
- `stop` - Set state to STOP
- `no_can` / `disconnect` - Simulate vehicle disconnect

### Contactor Commands

- `on` - Request contactor ON
- `off` - Request contactor OFF

### Enable Commands

- `enable` - Send enable=true to wallbox
- `disable` - Send enable=false to wallbox (also forces IDLE + contactor OFF)

### Info Commands

- `status` - Show simulator's current state
- `getudp` - Show UDP configuration
- `help` - Show command list

### Removed Commands

- ~~`getwb`~~ - No longer available (wallbox status not tracked)

## Files Modified

- `/Users/achraf/pro/PJMT/WallboxCtrl/src/simulator.cpp`
  - Removed wallbox status variables (global state)
  - Enhanced recv_cmd() function with feedback display
  - Added local static variables for change detection only
  - Removed validation checks
  - Removed print_wallbox_status() function
  - Removed getwb command

## Build

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
make simulator
```

## Related Documentation

- `docs/STATUS_EXCHANGE_FIX.md` - Previous bidirectional sync (now obsolete)
- `docs/SIMULATOR_STATUS_FIX.md` - Previous status fix (now obsolete)
- `docs/TEST_SYNC_GUIDE.md` - Previous sync testing (now obsolete)
- `INTERACTIVE_MODE.md` - Interactive mode guide
- `UDP_CONFIG_GUIDE.md` - UDP configuration

## Version History

### Version 2 (December 10, 2025)

- ✅ Added wallbox feedback display (enable/disable, relay state)
- ✅ Feedback messages shown without storing state
- ✅ Local static variables for change detection only
- ✅ User visibility into wallbox responses

### Version 1 (Initial)

- ✅ Removed all global wallbox status variables
- ✅ Removed state synchronization
- ✅ Removed command validation
- ✅ Simulator operates independently

## Notes

- Simulator maintains **independent operation** while showing wallbox feedback
- Feedback messages are **informational only** and don't affect command execution
- The wallbox state machine handles all logic internally
- Simulator sends commands without validation
- UDP communication works bidirectionally (100ms interval both directions)
- Wallbox status messages are received but not processed
