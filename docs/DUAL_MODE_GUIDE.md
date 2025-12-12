# Wallbox Dual Mode Guide

## Overview

The wallbox control system now supports **three operating modes**:

1. **API Mode** (default) - HTTP REST API only, no interactive terminal
2. **Interactive Mode** (`--interactive`) - Terminal commands only, no HTTP API
3. **Dual Mode** (`--dual`) - Both HTTP API and interactive terminal simultaneously

## Mode Comparison

| Feature                               | API Mode          | Interactive Mode    | Dual Mode                  |
| ------------------------------------- | ----------------- | ------------------- | -------------------------- |
| HTTP API Server (port 8080)           | ✅                | ❌                  | ✅                         |
| Interactive Terminal                  | ❌                | ✅                  | ✅                         |
| UDP Communication (ports 50010/50011) | ✅                | ✅                  | ✅                         |
| Background Process                    | ✅                | ⚠️ Limited          | ❌ Must stay in foreground |
| Resource Usage                        | Low               | Low                 | Medium                     |
| Use Case                              | Production server | Development/testing | Development/debugging      |

## Usage Examples

### API Mode (Production)

```bash
cd ~/wallbox-src/build
./wallbox_control_v3
# Server runs in foreground, press Ctrl+C to stop
# Or run in background:
./wallbox_control_v3 > /tmp/wallbox_api.log 2>&1 &
```

Access API at: `http://192.168.178.34:8080`

### Interactive Mode (Testing)

```bash
cd ~/wallbox-src/build
./wallbox_control_v3 --interactive
# Type commands: enable, disable, on, off, status, help, quit
```

### Dual Mode (Development/Debugging)

```bash
cd ~/wallbox-src/build
./wallbox_control_v3 --dual
# HTTP API runs in background thread
# Interactive terminal runs in main thread
# Type commands while API server is accessible
```

## Important Notes on UDP Port Conflicts

### The Problem

**UDP ports 50010 and 50011 can only be bound by ONE process at a time.** If you try to run multiple wallbox instances simultaneously (e.g., API mode + Interactive mode), you'll get UDP port conflicts and undefined behavior.

### Symptoms of Port Conflicts

- Commands like "on" not changing wallbox state
- UDP messages not being received
- Only one process gets the UDP packets (unpredictable which one)
- `lsof -i :50010` shows multiple processes

### Solution Options

#### Option 1: Run Single Process (Recommended)

Use **dual mode** to get both API and interactive terminal in ONE process:

```bash
./wallbox_control_v3 --dual
```

#### Option 2: Run Separate Processes (Advanced)

If you must run multiple instances (e.g., API on one machine, interactive on another):

1. **Kill all existing instances first:**

```bash
pkill -9 wallbox_control
```

2. **Verify no processes are running:**

```bash
ps aux | grep wallbox_control
lsof -i :50010  # Should show nothing
```

3. **Start only ONE instance:**

```bash
./wallbox_control_v3 --dual  # OR --interactive OR default (API)
```

#### Option 3: Use Different UDP Ports (Future Enhancement)

Currently, UDP ports are hardcoded. A future enhancement could allow multiple instances by:

- Making UDP ports configurable via config.json
- Using different UDP port pairs for each instance
- Example: Instance 1 on 50010/50011, Instance 2 on 50012/50013

## Architecture

### Dual Mode Implementation

```
┌─────────────────────────────────────────┐
│   Wallbox Control v3 (--dual)          │
│                                         │
│  ┌──────────────────────────────────┐ │
│  │  HTTP API Server                  │ │
│  │  (Background Thread)              │ │
│  │  - Port: 8080                     │ │
│  │  - Endpoints: /status, /enable,   │ │
│  │    /disable, /start, /stop, etc.  │ │
│  └──────────────────────────────────┘ │
│            │                            │
│            ├─ Shared Access             │
│            ↓                            │
│  ┌──────────────────────────────────┐ │
│  │  WallboxController (Shared)       │ │
│  │  - State Machine                  │ │
│  │  - GPIO Control                   │ │
│  │  - UDP Communication (50010/11)   │ │
│  └──────────────────────────────────┘ │
│            ↑                            │
│            ├─ Shared Access             │
│            │                            │
│  ┌──────────────────────────────────┐ │
│  │  Interactive Terminal             │ │
│  │  (Main Thread)                    │ │
│  │  - Commands: enable, disable, on, │ │
│  │    off, charge, status, help,quit │ │
│  └──────────────────────────────────┘ │
└─────────────────────────────────────────┘
```

### Key Points

1. **Single WallboxController Instance**: Both API and terminal share the same controller
2. **Thread-Safe Access**: HTTP server runs in background thread, interactive in main thread
3. **Single UDP Binding**: Only ONE UDP socket binding for ports 50010/50011
4. **Graceful Shutdown**: Typing "quit" in terminal stops both API and terminal

## Troubleshooting

### Problem: "Syntax error: word unexpected" when running wallbox_control_v3

**Cause**: Binary was compiled on macOS and copied to Banana Pi (architecture mismatch)

**Solution**: Compile directly on Banana Pi:

```bash
ssh bananapi
cd ~/wallbox-src/build
make wallbox_control_v3
```

### Problem: UDP commands not working in dual mode

**Checklist**:

1. Only ONE wallbox instance running?

```bash
ps aux | grep wallbox_control
# Should show only ONE process
```

2. UDP port not already bound?

```bash
lsof -i :50010
# Should show only wallbox_control_v3 process
```

3. Simulator configured with correct IP/ports?

```bash
# In simulator:
setudp 192.168.178.34 50011 50010
```

### Problem: Can't run dual mode in background

**Explanation**: Dual mode requires terminal interaction (stdin), so it can't run as a daemon.

**Solutions**:

- Use `screen` or `tmux` to keep it running in a persistent session
- Use API mode for background operation
- Create a systemd service for API mode

### Problem: Multiple wallbox processes causing conflicts

**Solution**:

```bash
# 1. Find all wallbox processes
ps aux | grep wallbox_control | grep -v grep

# 2. Kill them all
pkill -9 wallbox_control

# 3. Verify they're gone
ps aux | grep wallbox_control | grep -v grep

# 4. Check UDP ports are free
lsof -i :50010

# 5. Start fresh instance
cd ~/wallbox-src/build
./wallbox_control_v3 --dual
```

## Testing Dual Mode

### Test 1: Verify HTTP API Works

```bash
# In terminal 1 (on Banana Pi):
cd ~/wallbox-src/build
./wallbox_control_v3 --dual

# In terminal 2 (on Mac):
curl http://192.168.178.34:8080/api/status
```

### Test 2: Verify Interactive Terminal Works

```bash
# In terminal running dual mode:
> status
# Should show current state

> enable
# Should enable wallbox

> status
# Should show enabled state
```

### Test 3: Verify UDP Communication Works

```bash
# Start dual mode:
./wallbox_control_v3 --dual

# In simulator (on Mac):
setudp 192.168.178.34 50011 50010
on
status
# Should show "State 2" (ON)
```

### Test 4: Verify Both Interfaces Work Together

```bash
# Start dual mode
./wallbox_control_v3 --dual

# Via API (from Mac):
curl http://192.168.178.34:8080/api/start

# Via terminal (in dual mode):
> status
# Should show CHARGING state

# Via UDP (from simulator):
pause
# Should pause charging

# Via API (check state changed):
curl http://192.168.178.34:8080/api/status
# Should show PAUSED
```

## Best Practices

### Development

- Use **dual mode** for interactive debugging while testing API endpoints
- Keep simulator running to test UDP communication
- Monitor `/tmp/wallbox_v3.log` for detailed logs

### Testing

- Use **interactive mode** for quick manual testing
- Use **API mode** for automated testing scripts
- Always kill old processes before starting new tests

### Production

- Use **API mode** only (default)
- Run as systemd service with auto-restart
- Monitor logs and system resources
- Never run multiple instances on same UDP ports

## Code Changes Summary

The dual mode required these changes:

### 1. main_v3.cpp

```cpp
// Added dual mode flag parsing
bool dualMode = false;
if (arg == "--dual" || arg == "-d") {
    dualMode = true;
}

// Added dual mode initialization
if (!g_application->initialize(interactiveMode, dualMode))

// Added dual mode execution
if (dualMode) {
    g_application->runDual();
}
```

### 2. Application.h

```cpp
// Added m_dualMode member variable
bool m_dualMode;

// Updated initialize() signature
bool initialize(bool interactiveMode = false, bool dualMode = false)

// Modified API server initialization
if (!m_interactiveMode || m_dualMode) {
    // Start API server
}

// Added runDual() method
void runDual() {
    // API server already running in background
    // Run interactive terminal in main thread
}
```

## Future Enhancements

1. **Configurable UDP Ports**: Allow different instances to use different UDP ports
2. **Remote Interactive Client**: Connect to running wallbox via TCP/IP instead of local terminal
3. **Multi-Instance Support**: Coordination between multiple wallbox controllers
4. **Web-Based Terminal**: Interactive terminal accessible via browser (WebSocket)
5. **Dual Mode Background**: Use PTY or separate process for interactive terminal

## Related Documentation

- [Interactive Mode Implementation](./INTERACTIVE_MODE_IMPLEMENTATION.md)
- [Mode Differences FAQ](./MODE_DIFFERENCES_FAQ.md)
- [Project Summary](../PROJECT_SUMMARY.md)
- [Architecture](./architecture/ARCHITECTURE_V3.md)
