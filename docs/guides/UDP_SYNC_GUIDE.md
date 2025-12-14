# UDP Port & Address Synchronization Guide

## Quick Fix

### Problem

UDP communication between wallbox and simulator not working? Ports likely reversed!

### Solution

**On Banana Pi** (`config.json`):

```json
{
  "network": {
    "udp_listen_port": 50010,           ← Wallbox receives commands here
    "udp_send_port": 50011,              ← Wallbox sends responses here
    "udp_send_address": "192.168.178.23" ← Mac IP address
  }
}
```

**In Simulator** (Mac):

```bash
./simulator
> setudp 192.168.178.34 50011 50010
#        ^^^^^^^^^^^^^^^^ ^^^^^ ^^^^^
#        Banana Pi IP     send  receive
```

The simulator will:

- Send commands to `192.168.178.34:50010`
- Receive responses on local port `50011`

## Understanding the Port Mapping

Think of it like a **phone call**:

- Wallbox phone number: `192.168.178.34:50010`
- Simulator phone number: `192.168.178.23:50011`

```
┌─────────────────────┐              ┌─────────────────────┐
│   Simulator (Mac)   │              │ Wallbox (Banana Pi) │
│  192.168.178.23     │              │  192.168.178.34     │
│                     │              │                     │
│  Listen: 50011  ◄───┼──────────────┼─── Send: 50011      │
│  Send:   50010  ───-┼──────────────┼───► Listen: 50010   │
└─────────────────────┘              └─────────────────────┘
       Commands →                           ← Responses
```

## Step-by-Step Setup

### 1. Configure Wallbox (One-Time Setup)

Edit `/path/to/project/WallboxCtrl/build/config.json`:

```json
{
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "192.168.178.23",
    "api_port": 8080
  }
}
```

Copy to Banana Pi:

```bash
scp /path/to/project/WallboxCtrl/build/config.json bananapi:~/wallbox-src/build/
```

### 2. Start Wallbox

```bash
ssh bananapi
cd ~/wallbox-src/build
./wallbox_control_v3
```

You should see:

```
UDP Listen Port: 50010
UDP Send Port: 50011
UDP Send Address: 192.168.178.23
```

### 3. Configure Simulator (Every Time)

```bash
cd /path/to/project/WallboxCtrl/build
./simulator

# In simulator:
> setudp 192.168.178.34 50011 50010
✓ UDP configuration updated to: 192.168.178.34 50011 -> 50010

> status
```

### 4. Test Communication

```bash
# In simulator:
> on
✓ Main contactor turned ON

> status
Main Contactor: ON
Charging State: idle
```

## Common Mistakes

### ❌ Mistake 1: Wrong IP in config.json

```json
"udp_send_address": "192.168.178.34"  // WRONG - This is wallbox's own IP!
```

✅ Should be:

```json
"udp_send_address": "192.168.178.23"  // CORRECT - Simulator's IP
```

### ❌ Mistake 2: Ports Reversed in config.json

```json
"udp_listen_port": 50011,  // WRONG
"udp_send_port": 50010,    // WRONG
```

✅ Should be:

```json
"udp_listen_port": 50010,  // CORRECT
"udp_send_port": 50011,    // CORRECT
```

### ❌ Mistake 3: Wrong simulator setudp command

```bash
> setudp 192.168.178.23 50010 50011  # WRONG - Using Mac's IP
```

✅ Should be:

```bash
> setudp 192.168.178.34 50011 50010  # CORRECT - Using Banana Pi's IP
```

## Troubleshooting

### Commands Not Reaching Wallbox

**Check 1: Correct ports?**

```bash
# On Banana Pi:
ssh bananapi 'lsof -i :50010'
# Should show: wallbox_control_v3 LISTEN on *:50010

# On Mac:
lsof -i :50011
# Should show: simulator LISTEN on *:50011
```

**Check 2: Firewall blocking?**

```bash
# On Banana Pi - allow UDP 50010:
ssh bananapi 'sudo ufw allow 50010/udp'
ssh bananapi 'sudo ufw allow 50011/udp'
```

**Check 3: Wrong IP address?**

```bash
# Verify Mac IP:
ifconfig | grep "inet " | grep -v 127.0.0.1

# Verify Banana Pi IP:
ssh bananapi 'hostname -I'
```

### Responses Not Reaching Simulator

**Check simulator UDP config:**

```bash
# In simulator:
> getudp
UDP Address: 192.168.178.34
UDP In Port: 50011
UDP Out Port: 50010
```

Should match:

- Address: `192.168.178.34` (Banana Pi)
- In Port: `50011` (where simulator listens)
- Out Port: `50010` (where wallbox listens)

### Multiple Processes Causing Conflicts

```bash
# Check for multiple wallbox instances:
ssh bananapi 'ps aux | grep wallbox_control | grep -v grep'

# Kill all:
ssh bananapi 'pkill -9 wallbox_control'

# Start fresh:
ssh bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3'
```

## Port Configuration Reference

| Component               | Listen Port | Send Port | Send Address               |
| ----------------------- | ----------- | --------- | -------------------------- |
| **Wallbox** (Banana Pi) | 50010       | 50011     | 192.168.178.23 (Mac)       |
| **Simulator** (Mac)     | 50011       | 50010     | 192.168.178.34 (Banana Pi) |

### Why These Specific Ports?

- **50010**: Standard port for ISO 15118 communication (wallbox listens)
- **50011**: Response port for ISO 15118 communication (simulator listens)
- These ports are part of the ISO 15118 protocol specification

## Alternative: Change Ports

If you need different ports (e.g., port conflict):

### Change Wallbox Ports

Edit `config.json`:

```json
{
  "network": {
    "udp_listen_port": 60010, // Changed from 50010
    "udp_send_port": 60011, // Changed from 50011
    "udp_send_address": "192.168.178.23"
  }
}
```

### Update Simulator

```bash
> setudp 192.168.178.34 60011 60010
```

**Important**: Ports must be:

- Between 1024 and 65535
- Not already in use by another application
- Mirrored between wallbox and simulator

## Files to Check

### Wallbox Configuration

```bash
# On Banana Pi:
cat ~/wallbox-src/build/config.json
```

### Simulator Configuration

The simulator stores UDP config in memory (not persisted).
Always use `setudp` command after starting simulator.

### Check Network Connectivity

```bash
# Ping Banana Pi from Mac:
ping 192.168.178.34

# Ping Mac from Banana Pi:
ssh bananapi 'ping -c 3 192.168.178.23'
```

## Quick Test Commands

### Full Test Sequence

```bash
# Terminal 1 - Start Wallbox
ssh bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3'

# Terminal 2 - Test Simulator
cd /path/to/project/WallboxCtrl/build
./simulator
> setudp 192.168.178.34 50011 50010
> on
> charge
> status
> quit
```

Expected output:

```
✓ UDP configuration updated to: 192.168.178.34 50011 -> 50010
✓ Main contactor turned ON
✓ Charging state changed to: CHARGING
Main Contactor: ON
Charging State: charging
```

## Summary

**Key Rule**: Wallbox's send port = Simulator's listen port (and vice versa)

```
Wallbox sends on 50011 → Simulator listens on 50011 ✓
Simulator sends on 50010 → Wallbox listens on 50010 ✓
```

When in doubt:

1. Check `config.json` on Banana Pi
2. Use `setudp 192.168.178.34 50011 50010` in simulator
3. Test with `> status` command

## Related Documentation

- [Dual Mode Guide](./DUAL_MODE_GUIDE.md)
- [Interactive Mode Implementation](./INTERACTIVE_MODE_IMPLEMENTATION.md)
- [Mode Differences FAQ](./MODE_DIFFERENCES_FAQ.md)
