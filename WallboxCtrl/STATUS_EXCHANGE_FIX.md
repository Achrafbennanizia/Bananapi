# Status Exchange & State Synchronization - Complete Fix

## Problem Summary

The wallbox and simulator had multiple communication and synchronization issues:

1. Delayed state synchronization (500ms vs 100ms intervals)
2. Simulator state changes not reflected in wallbox state machine
3. Relay state changes not syncing simulator contactor state
4. Missed status updates during state transitions
5. **Wallbox state changes not sent to simulator** (NEW)
6. **Simulator couldn't synchronize with wallbox state** (NEW)

## Root Causes

### 1. **Timing Mismatch**

- **Simulator**: Sent status every 100ms
- **Wallbox**: Sent status every 500ms
- **Impact**: Simulator could change state 5 times before wallbox sent an update

### 2. **One-Way Communication**

- **Wallbox → Simulator**: Only sent enable/relay status, didn't process simulator state
- **Simulator → Wallbox**: Sent state updates, but wallbox only logged them without acting
- **Impact**: State changes from simulator were ignored by wallbox

### 3. **No State Synchronization**

- Simulator could be in "charging" state while wallbox was in "idle"
- Relay changes didn't update simulator contactor state
- No automatic state alignment

## Solutions Implemented

### 1. **Synchronized Timing** ✅

```cpp
// Before: 500ms interval
const auto statusInterval = std::chrono::milliseconds(500);

// After: 100ms interval (matches simulator)
const auto statusInterval = std::chrono::milliseconds(100);
```

**Benefit**: Both systems now exchange status at the same frequency (10 times per second)

### 2. **Bidirectional State Synchronization** ✅

#### Wallbox Now Processes Simulator State:

```cpp
void WallboxController::processNetworkMessage(const std::vector<uint8_t> &message)
{
    // Update wallbox state machine based on simulator state
    switch (state.isoStackState.state)
    {
    case enIsoChargingState::idle:
        m_stateMachine->stopCharging("Simulator state: idle");
        break;
    case enIsoChargingState::charging:
        m_stateMachine->startCharging("Simulator state: charging");
        break;
    // ... other states
    }

    // Send immediate status update when state changes
    sendStatusToSimulator();
}
```

**Benefit**: Wallbox now responds to simulator state changes in real-time

#### Simulator Now Syncs Contactor with Relay:

```cpp
void recv_cmd(int sock)
{
    // Sync simulator contactor with wallbox relay
    g_mainContactorCmd = relayState;

    // Auto-transition to IDLE when relay turns off
    if (!relayState && (g_chargingState == enIsoChargingState::charging ||
                        g_chargingState == enIsoChargingState::ready))
    {
        g_chargingState = enIsoChargingState::idle;
    }
}
```

**Benefit**: Simulator contactor follows wallbox relay state automatically

### 3. **Immediate Status Updates on State Changes** ✅

- Both systems now send immediate status updates when state changes occur
- No need to wait for the next periodic update
- Ensures tight synchronization during transitions

### 4. **Wallbox-to-Simulator State Encoding** ✅ (NEW)

The wallbox now encodes its charging state in the `currentDemand` field:

```cpp
void WallboxController::sendStatusToSimulator()
{
    // Map wallbox charging state to current demand (use as state indicator)
    ChargingState currentState = m_stateMachine->getCurrentState();
    switch (currentState)
    {
    case ChargingState::IDLE:
        cmd.isoStackCmd.currentDemand = 0;      // 0 = idle
        break;
    case ChargingState::PREPARING:
        cmd.isoStackCmd.currentDemand = 100;    // 100 = preparing/ready
        break;
    case ChargingState::CHARGING:
        cmd.isoStackCmd.currentDemand = 160;    // 160 = charging (16.0A)
        break;
    case ChargingState::PAUSED:
        cmd.isoStackCmd.currentDemand = 50;     // 50 = paused
        break;
    case ChargingState::FINISHING:
        cmd.isoStackCmd.currentDemand = 10;     // 10 = finishing/stop
        break;
    }
}
```

**Benefit**: Wallbox state is now communicated to simulator in every status message

### 5. **Simulator State Decoding & Auto-Sync** ✅ (NEW)

The simulator decodes wallbox state and automatically synchronizes:

```cpp
void recv_cmd(int sock)
{
    // Decode wallbox state from currentDemand field
    enIsoChargingState wallboxDesiredState = g_chargingState;
    if (currentDemand == 0)
        wallboxDesiredState = enIsoChargingState::idle;
    else if (currentDemand == 100)
        wallboxDesiredState = enIsoChargingState::ready;
    else if (currentDemand >= 160)
        wallboxDesiredState = enIsoChargingState::charging;
    else if (currentDemand == 50)
        wallboxDesiredState = enIsoChargingState::ready;
    else if (currentDemand == 10)
        wallboxDesiredState = enIsoChargingState::stop;

    // Sync simulator state with wallbox desired state (if enabled)
    if (enableState && wallboxDesiredState != g_chargingState)
    {
        g_chargingState = wallboxDesiredState;
    }
}
```

**Benefit**: Simulator automatically mirrors wallbox state changes

## Testing Instructions

### Quick Test:

```bash
# Terminal 1
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./simulator

# Terminal 2 (in a new terminal)
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./wallbox_control_v3
```

### Test Scenarios:

#### Scenario 1: Simulator-Initiated Charging

```
Simulator> ready
  → Wallbox should show: [SIMULATOR → WALLBOX] State: idle → ready

Simulator> charge
  → Wallbox should show: [SIMULATOR → WALLBOX] State: ready → charging
  → Wallbox state machine should transition to CHARGING
```

#### Scenario 2: Wallbox-Initiated Charging

```
Wallbox> start
  → Simulator should show: [WALLBOX → SIMULATOR] Relay changed: OFF → ON
  → Simulator contactor should turn ON automatically

Wallbox> stop
  → Simulator should show: [WALLBOX → SIMULATOR] Relay changed: ON → OFF
  → Simulator should auto-transition to IDLE
```

#### Scenario 3: Enable/Disable Synchronization

```
Wallbox> disable
  → Simulator commands should be rejected

Simulator> charge
  → Should show: ⚠️ Cannot execute 'charge': Wallbox enable=false

Wallbox> enable
  → Simulator commands work again
```

#### Scenario 4: Wallbox State Changes Auto-Sync to Simulator (NEW)

```
Wallbox> start
  → Simulator shows: [WALLBOX → SIMULATOR] State sync request: idle → charging
  → Simulator automatically changes to CHARGING state

Wallbox> pause
  → Simulator shows: [WALLBOX → SIMULATOR] State sync request: charging → ready
  → Simulator automatically changes to READY state

Wallbox> stop
  → Simulator shows: [WALLBOX → SIMULATOR] State sync request: ready → idle
  → Simulator automatically changes to IDLE state
```

**Key Feature**: The simulator now **automatically follows** wallbox state changes!

## Status Exchange Flow

### Before All Fixes:

```
Wallbox (500ms) ----enable/relay----> Simulator (100ms)
                                      ↓
                                   (ignored)

Simulator ----state/contactor----> Wallbox
                                      ↓
                                   (logged but not processed)

No state synchronization in either direction!
```

### After All Fixes:

```
Wallbox (100ms) ====enable/relay/STATE====> Simulator (100ms)
      ↑                                            ↓
      |                                   syncs contactor
      |                                   auto-syncs state
      |                                   displays changes
      |
      |<====state/contactor============= Simulator
      ↓
updates state machine
sends immediate response
auto-syncs with simulator

BIDIRECTIONAL STATE SYNCHRONIZATION!
```

### State Encoding Protocol:

**Wallbox → Simulator** (via `currentDemand` field):

- `0` = IDLE
- `10` = FINISHING/STOP
- `50` = PAUSED → READY
- `100` = PREPARING → READY
- `160+` = CHARGING

**Simulator → Wallbox** (via `isoStackState.state`):

- `idle` → Wallbox IDLE
- `ready` → Wallbox PREPARING
- `charging` → Wallbox CHARGING
- `stop` → Wallbox FINISHING

## Expected Console Output

### Successful Status Exchange:

```
[WALLBOX → SIMULATOR] 🔌 Relay state changed: OFF → ✅ ON
[SIMULATOR] Auto-syncing contactor to relay state

[SIMULATOR → WALLBOX] State: idle → ready  Contactor: OFF → ON
[WALLBOX] State machine transitioning: IDLE → CHARGING

[WALLBOX → SIMULATOR] Sending enable status: ENABLED
[WALLBOX → SIMULATOR] Sending state change: IDLE → CHARGING
[SIMULATOR] State sync request: idle → charging
```

### New Feature - Wallbox Controls Simulator (NEW):

```
# In wallbox terminal:
> start

# Simulator automatically shows:
[WALLBOX → SIMULATOR] 🔌 Relay state changed: OFF → ✅ ON
[WALLBOX → SIMULATOR] State sync request: idle → charging

# In wallbox terminal:
> pause

# Simulator automatically shows:
[WALLBOX → SIMULATOR] State sync request: charging → ready

# In wallbox terminal:
> stop

# Simulator automatically shows:
[WALLBOX → SIMULATOR] 🔌 Relay state changed: ON → ❌ OFF
[WALLBOX → SIMULATOR] State sync request: ready → idle
[SIMULATOR] Auto-transitioning to IDLE due to relay OFF
```

## Verification Checklist

✅ Status messages appear every 100ms (check logs)  
✅ Simulator "ready" command updates wallbox state  
✅ Simulator "charge" command starts wallbox charging  
✅ Wallbox "start" turns on simulator contactor  
✅ Wallbox "stop" transitions simulator to idle  
✅ Relay OFF auto-transitions simulator to idle  
✅ Wallbox disable blocks simulator commands  
✅ No delays in state synchronization  
✅ Both log files show synchronized timestamps  
✅ **Wallbox "start" auto-changes simulator to CHARGING** (NEW)  
✅ **Wallbox "pause" auto-changes simulator to READY** (NEW)  
✅ **Wallbox "stop" auto-changes simulator to IDLE** (NEW)  
✅ **Wallbox state changes appear in simulator console** (NEW)

## Log Files

Monitor these files during testing:

- Wallbox: `/tmp/wallbox_v3.log`
- Simulator: `/tmp/wallbox_simulator.log`

```bash
# Monitor both logs simultaneously
tail -f /tmp/wallbox_v3.log /tmp/wallbox_simulator.log
```

## Performance Improvements

| Metric                      | Before        | After         | Improvement     |
| --------------------------- | ------------- | ------------- | --------------- |
| Status send interval        | 500ms / 100ms | 100ms / 100ms | Synchronized    |
| Max state sync delay        | 500ms         | 100ms         | **5x faster**   |
| State change response       | Ignored       | Immediate     | **Real-time**   |
| Relay-contactor sync        | Manual        | Automatic     | **Automated**   |
| Wallbox→Simulator state     | Not sent      | Encoded       | **NEW Feature** |
| Simulator auto-sync         | No            | Yes           | **NEW Feature** |
| Bidirectional state control | No            | Yes           | **Full Sync**   |

## Known Limitations

1. **Network latency not handled**: Assumes local UDP is instant
2. **No retry mechanism**: Lost packets are not retransmitted
3. **Single-threaded**: Status processing happens in main loop

## Future Enhancements

- Add message acknowledgment for critical state changes
- Implement exponential backoff for failed communications
- Add connection health monitoring
- Support for redundant communication paths

## Files Modified

1. `/Users/achraf/pro/PJMT/WallboxCtrl/src/WallboxController.cpp`

   - Reduced status interval: 500ms → 100ms
   - Added state synchronization logic
   - Added immediate status response on state changes
   - **Added wallbox state encoding in currentDemand field** (NEW)
   - **Added state change notifications to simulator** (NEW)
   - **Added debug logging for state transmission** (NEW)

2. `/Users/achraf/pro/PJMT/WallboxCtrl/src/simulator.cpp`

   - Added contactor-relay synchronization
   - Added auto-transition to IDLE on relay OFF
   - Improved state change logging
   - **Fixed initial relay state from true to false** (FIXED)
   - **Added first message detection** (NEW)
   - **Added wallbox state decoding from currentDemand** (NEW)
   - **Added automatic state synchronization with wallbox** (NEW)
   - **Added state sync request notifications** (NEW)

3. **Created: `/Users/achraf/pro/PJMT/WallboxCtrl/test_state_sync.sh`** (NEW)
   - Test script for state synchronization
   - Demonstrates bidirectional control
   - Instructions for testing all scenarios

## Build and Deploy

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
cmake ..
make
```

Both `wallbox_control_v3` and `simulator` executables are now updated with the fixes.

## Complete Feature Summary

### Bidirectional Communication Achieved ✅

**Simulator → Wallbox:**

- Simulator state commands update wallbox state machine
- Contactor commands control wallbox relay
- Real-time synchronization

**Wallbox → Simulator:**

- Wallbox state changes update simulator automatically
- Relay changes sync simulator contactor
- Enable/disable status synchronized
- **NEW: State machine transitions encoded and transmitted**

### State Mapping

| Wallbox State | currentDemand Value | Simulator State |
| ------------- | ------------------- | --------------- |
| IDLE          | 0                   | idle            |
| PREPARING     | 100                 | ready           |
| CHARGING      | 160                 | charging        |
| PAUSED        | 50                  | ready           |
| FINISHING     | 10                  | stop            |

This creates a **fully synchronized** system where either component can initiate state changes!
