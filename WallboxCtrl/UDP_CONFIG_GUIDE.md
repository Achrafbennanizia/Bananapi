# UDP Configuration & Real-time Feedback - v3 & Simulator

## ✨ New Features (December 2025)

### 1. UDP Configuration Commands

Change UDP address and ports dynamically without editing code.

### 2. Real-time Status Feedback

Both terminals show live feedback when states change.

### 3. Wallbox Disable Protection

All charging and control commands blocked when wallbox is disabled with clear feedback.

**How it works:**

- Wallbox continuously sends its enable/disable status via UDP (every 500ms)
- Simulator reads this status from incoming UDP packets
- All simulator commands are blocked when wallbox reports `enable=false`

**Blocked commands when disabled:**

- Wallbox: `start`, `pause`, `resume`
- Simulator: `on`, `off`, `no_can`, `idle`, `ready`, `charge`, `stop`

### 4. State Change Notifications

Automatic notifications when charging states transition (idle → ready → charging).

### 5. No Vehicle Simulation

New `no_can` command to simulate disconnected vehicle state.

---

## New Commands Added

### Wallbox Controller v3

#### View UDP Configuration

```bash
> getudp
```

Shows current UDP settings:

- Listen Port
- Send Port
- Send Address

#### Change UDP Configuration

```bash
> setudp <address> <listen_port> <send_port>
```

**Example:**

```bash
> setudp 192.168.1.100 50020 50021
```

**Note:** Changes require restart to take effect.

---

### Simulator

#### View UDP Configuration

```bash
> getudp
```

Shows:

- Target Address
- Listen Port (in)
- Send Port (out)

#### Change UDP Configuration

```bash
> setudp <address> <in_port> <out_port>
```

**Example:**

```bash
> setudp 192.168.1.50 50021 50020
```

**Note:** Port changes require simulator restart to rebind sockets.

---

## Complete Command Reference

### Wallbox Controller v3

| Command                             | Description                |
| ----------------------------------- | -------------------------- |
| `enable`                            | Enable wallbox             |
| `disable`                           | Disable wallbox            |
| `start`                             | Start charging             |
| `stop`                              | Stop charging              |
| `pause`                             | Pause charging             |
| `resume`                            | Resume charging            |
| `status`                            | Show current status        |
| `setrelay <pin>`                    | Change relay pin (0-27)    |
| `getpins`                           | Show pin configuration     |
| **`getudp`**                        | **Show UDP configuration** |
| **`setudp <addr> <listen> <send>`** | **Set UDP config**         |
| `help`                              | Show help                  |
| `quit`                              | Exit                       |

### Simulator

| Command                        | Description                                      |
| ------------------------------ | ------------------------------------------------ |
| `on`                           | Turn main contactor ON                           |
| `off`                          | Turn main contactor OFF                          |
| **`no_can`**                   | **Simulate no vehicle connected (disconnect)**   |
| `idle`                         | Set state to IDLE (vehicle plugged, no charging) |
| `ready`                        | Set state to READY (ready to charge)             |
| `charge`                       | Set state to CHARGING (power transfer active)    |
| `stop`                         | Set state to STOP (stopping session)             |
| `status`                       | Show current status with descriptions            |
| **`getudp`**                   | **Show UDP configuration**                       |
| **`setudp <addr> <in> <out>`** | **Set UDP config**                               |
| `help`                         | Show help                                        |
| `quit`                         | Exit                                             |
| **`setudp <addr> <in> <out>`** | **Set UDP config**                               |
| `help`                         | Show help                                        |
| `quit`                         | Exit                                             |

---

## Usage Examples

### Simulate Vehicle Connection Sequence

```bash
# In simulator
> no_can
✓ Simulating: NO VEHICLE CONNECTED

> on
✓ Main contactor turned ON
[SIMULATOR → WALLBOX] Sending contactor ON command

> idle
✓ Charging state changed to: IDLE (Vehicle plugged, no charging)
[SIMULATOR → WALLBOX] Sending state: IDLE

> ready
✓ Charging state changed to: READY (Vehicle ready to charge)
[SIMULATOR] 📊 State transition: idle → ready

> charge
✓ Charging state changed to: CHARGING (Power transfer active)
[SIMULATOR] 📊 State transition: ready → charging
```

### Test Wallbox Disable Protection

```bash
# In wallbox
> disable
[WALLBOX] 🔴 Wallbox DISABLED - All charging commands blocked

> start
⚠️  Cannot start charging: wallbox is disabled
[WALLBOX] ❌ Command rejected - wallbox disabled

> enable
[WALLBOX] 🟢 Wallbox ENABLED - Ready for charging

> start
[WALLBOX → SIMULATOR] ✓ Starting charging sequence
```

### Change Wallbox to custom IP

```bash
# In wallbox_control_v3
> getudp
UDP Configuration:
  Listen Port: 50010
  Send Port: 50011
  Send Address: 127.0.0.1

> setudp 192.168.1.100 50010 50011
✓ UDP config will be: 192.168.1.100:50010 -> 50011
  (Restart required to apply changes)

> quit
```

### Change Simulator ports

```bash
# In simulator
> getudp
UDP Configuration:
  Target Address: 127.0.0.1
  Listen Port (in): 50011
  Send Port (out): 50010

> setudp 192.168.1.100 50021 50020
✓ UDP configuration updated to: 192.168.1.100 50021 -> 50020
  (Restart simulator to rebind ports)

> quit
```

---

## Port Configuration Rules

1. **Port numbers**: 1-65535
2. **Listen Port**: Where the application receives UDP messages
3. **Send Port**: Where the application sends UDP messages
4. **Wallbox vs Simulator**: Ports must be swapped
   - Wallbox listen = Simulator send
   - Wallbox send = Simulator listen

### Default Configuration

```
Wallbox:   Listen: 50010, Send: 50011, Target: 127.0.0.1
Simulator: Listen: 50011, Send: 50010, Target: 127.0.0.1
```

### Custom Network Example

```
Wallbox (192.168.1.100):
  setudp 192.168.1.50 50010 50011

Simulator (192.168.1.50):
  setudp 192.168.1.100 50011 50010
```

---

## Real-time Feedback Examples

### Wallbox Status Changes

```
[WALLBOX] 🟢 Wallbox ENABLED - Ready for charging
[WALLBOX] 🔴 Wallbox DISABLED - All charging commands blocked
```

### When Wallbox is Disabled

```
# In wallbox terminal:
> start
⚠️  Cannot start charging: wallbox is disabled
[WALLBOX] ❌ Command rejected - wallbox disabled

# In simulator terminal:
[WALLBOX → SIMULATOR] 🔴 WALLBOX DISABLED - Charging blocked

# Try any simulator command - all are blocked:
> on
⚠️  Cannot turn contactor ON: wallbox is disabled
[SIMULATOR] ❌ Command rejected - wallbox disabled

> ready
⚠️  Cannot change to READY state: wallbox is disabled
[SIMULATOR] ❌ Command rejected - wallbox disabled

> no_can
⚠️  Cannot simulate vehicle disconnect: wallbox is disabled
[SIMULATOR] ❌ Command rejected - wallbox disabled
```

### Relay State Changes

```
# In wallbox terminal:
[WALLBOX → SIMULATOR] Relay state: ON

# In simulator terminal:
[WALLBOX → SIMULATOR] 🔌 Relay state changed: OFF → ✅ ON
```

### State Transitions (Simulator)

```
[SIMULATOR] 📊 State transition: idle → ready
[SIMULATOR] 📊 State transition: ready → charging
[SIMULATOR] 📊 State transition: charging → stop
```

### No Vehicle Connected

```
> no_can
✓ Simulating: NO VEHICLE CONNECTED
[SIMULATOR → WALLBOX] Vehicle disconnected - State: IDLE, Contactor: OFF

> status
--- Current Status ---
Main Contactor: OFF
Charging State: idle (No vehicle connected)
UDP Address: 127.0.0.1
UDP In Port: 50011
UDP Out Port: 50010
```

---

## Usage Examples

### Simulate Vehicle Connection Sequence

```bash
# In simulator
> no_can
✓ Simulating: NO VEHICLE CONNECTED

> on
✓ Main contactor turned ON
[SIMULATOR → WALLBOX] Sending contactor ON command

> idle
✓ Charging state changed to: IDLE (Vehicle plugged, no charging)
[SIMULATOR → WALLBOX] Sending state: IDLE

> ready
✓ Charging state changed to: READY (Vehicle ready to charge)
[SIMULATOR] 📊 State transition: idle → ready

> charge
✓ Charging state changed to: CHARGING (Power transfer active)
[SIMULATOR] 📊 State transition: ready → charging
```

### Test Wallbox Disable Protection

```bash
# In wallbox
> disable
[WALLBOX] 🔴 Wallbox DISABLED - All charging commands blocked

> start
⚠️  Cannot start charging: wallbox is disabled
[WALLBOX] ❌ Command rejected - wallbox disabled

> enable
[WALLBOX] 🟢 Wallbox ENABLED - Ready for charging

> start
[WALLBOX → SIMULATOR] ✓ Starting charging sequence
```

### Change UDP Configuration

```bash
# Change Wallbox to custom IP
> getudp
UDP Configuration:
  Listen Port: 50010
  Send Port: 50011
  Send Address: 127.0.0.1

> setudp 192.168.1.100 50010 50011
✓ UDP config will be: 192.168.1.100:50010 -> 50011
  (Restart required to apply changes)
```

---

## Summary of All Changes

### Wallbox Controller v3

✅ UDP configuration commands (`getudp`, `setudp`)
✅ Real-time relay state feedback
✅ Wallbox enable/disable status messages
✅ Command blocking when disabled (start, pause, resume)
✅ Clear warning messages with emojis

### Simulator

✅ UDP configuration commands (`getudp`, `setudp`)
✅ Automatic state transition notifications
✅ No vehicle simulation (`no_can` command)
✅ Enhanced status display with descriptions
✅ Relay change notifications
✅ Wallbox enable/disable feedback
✅ Command blocking when disabled (on, off, no_can, idle, ready, charge, stop)
✅ Clear state descriptions in status

### Communication Feedback

✅ Bidirectional status updates
✅ Real-time notifications on both terminals
✅ Clear visual indicators (🟢🔴⚠️✅❌🔌📊)
✅ Descriptive state messages

```
Wallbox                    Simulator
--------                   ---------
enable/disable command
↓
m_wallboxEnabled = true/false
↓
sendStatusToSimulator()    ← UDP (every 500ms)
(enable field in packet)   ↓
                           g_lastEnableState updated
                           ↓
                           ensure_wallbox_enabled() checks
                           ↓
                           Commands allowed/blocked
```
