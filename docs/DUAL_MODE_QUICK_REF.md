# Quick Reference: Running Wallbox Without Conflicts

## ✅ SOLUTION: Use Dual Mode

Instead of running TWO separate processes (which causes UDP port conflicts), run ONE process with dual mode:

```bash
ssh bananapi
cd ~/wallbox-src/build
./wallbox_control_v3 --dual
```

This gives you:

- ✅ HTTP API on port 8080
- ✅ Interactive terminal for direct commands
- ✅ UDP communication on ports 50010/50011
- ✅ **NO PORT CONFLICTS** (single process = single UDP binding)

## Three Available Modes

| Command                              | Use Case                     | Can Run in Background? |
| ------------------------------------ | ---------------------------- | ---------------------- |
| `./wallbox_control_v3`               | Production API server        | ✅ Yes                 |
| `./wallbox_control_v3 --interactive` | Testing/debugging            | ⚠️ Limited             |
| `./wallbox_control_v3 --dual`        | Development (API + terminal) | ❌ No (needs stdin)    |

## Common Commands

### Start API Mode (Background)

```bash
ssh bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3 > /tmp/wallbox_api.log 2>&1 &'
```

### Start Interactive Mode

```bash
ssh bananapi
cd ~/wallbox-src/build
./wallbox_control_v3 --interactive
```

### Start Dual Mode

```bash
ssh bananapi
cd ~/wallbox-src/build
./wallbox_control_v3 --dual
```

### Kill All Wallbox Processes

```bash
ssh bananapi 'pkill -9 wallbox_control'
```

### Check Running Processes

```bash
ssh bananapi 'ps aux | grep wallbox_control | grep -v grep'
```

### Check UDP Port Usage

```bash
ssh bananapi 'lsof -i :50010'
```

## Troubleshooting

### Problem: UDP commands not working

**Check for multiple processes:**

```bash
ssh bananapi 'ps aux | grep wallbox_control | grep -v grep'
```

If you see MORE THAN ONE process, that's the problem!

**Fix:**

```bash
# Kill all processes
ssh bananapi 'pkill -9 wallbox_control'

# Start only one
ssh bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3 --dual'
```

### Problem: Port already in use

```bash
# Find what's using the port
ssh bananapi 'lsof -i :50010'

# Kill that specific process
ssh bananapi 'kill -9 <PID>'
```

## Testing UDP Communication

### From Simulator

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./simulator

# In simulator:
> setudp 192.168.178.34 50011 50010
> on
> status
> quit
```

### From Wallbox Terminal (Dual/Interactive Mode)

```bash
> enable
> status
> on
> charge
> status
> quit
```

## Testing HTTP API

```bash
# Get status
curl http://192.168.178.34:8080/api/status

# Enable wallbox
curl -X POST http://192.168.178.34:8080/api/enable

# Start charging
curl -X POST http://192.168.178.34:8080/api/start

# Get status again
curl http://192.168.178.34:8080/api/status
```

## Files Modified

1. `/Users/achraf/pro/PJMT/WallboxCtrl/src/main_v3.cpp` - Added dual mode flag parsing
2. `/Users/achraf/pro/PJMT/WallboxCtrl/include/Application.h` - Added runDual() method and m_dualMode variable

## Documentation Created

1. `docs/DUAL_MODE_GUIDE.md` - Complete guide with architecture diagrams
2. `docs/CONCURRENT_MODE_SOLUTION.md` - Detailed problem/solution analysis
3. `docs/DUAL_MODE_QUICK_REF.md` - This quick reference (you are here)

## Key Takeaway

**OLD WAY (❌ causes conflicts):**

```bash
# Terminal 1
./wallbox_control_api &

# Terminal 2
./wallbox_control_v3 --interactive

# Result: Both try to bind UDP port 50010 → CONFLICT
```

**NEW WAY (✅ no conflicts):**

```bash
# Single process with both features
./wallbox_control_v3 --dual

# Result: One process, one UDP binding → NO CONFLICT
```

## Compilation (Important!)

**Never copy macOS binary to Banana Pi!** Always compile on target:

```bash
# Update source files on Banana Pi
scp /Users/achraf/pro/PJMT/WallboxCtrl/src/main_v3.cpp bananapi:~/wallbox-src/src/
scp /Users/achraf/pro/PJMT/WallboxCtrl/include/Application.h bananapi:~/wallbox-src/src/

# Compile on Banana Pi
ssh bananapi 'cd ~/wallbox-src/build && make wallbox_control_v3'
```

## Related Documentation

- [Complete Dual Mode Guide](./DUAL_MODE_GUIDE.md)
- [Solution Analysis](./CONCURRENT_MODE_SOLUTION.md)
- [Interactive Mode Implementation](./INTERACTIVE_MODE_IMPLEMENTATION.md)
- [Mode Differences FAQ](./MODE_DIFFERENCES_FAQ.md)
- [Project Summary](../PROJECT_SUMMARY.md)
