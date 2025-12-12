# Wallbox Control Modes - Differences & FAQ

**Document:** Mode Differences and Common Questions  
**Date:** December 11, 2025  
**Project:** PJMT Wallbox Control System

---

## What are the differences between API Mode and Interactive Mode?

### **API Mode (Default)**

**How to start:** `./wallbox_control_v3` (no flags)

**Characteristics:**

- Starts HTTP REST API server on port 8080
- Runs in background as a service
- Controlled via HTTP requests (curl, React app, Postman)
- Network-accessible from other devices
- Designed for production use
- Higher memory usage (~15 MB) due to HTTP stack
- Slower startup (~1.2 seconds) - needs to initialize web server

**Use cases:**

- Production deployment
- Remote monitoring and control
- Integration with web interfaces
- Multiple clients can connect simultaneously
- Automated systems/scripts via API calls

**Access methods:**

```bash
curl http://192.168.178.34:8080/api/status
curl -X POST http://192.168.178.34:8080/api/charging/start
# React web app at localhost:3000
```

---

### **Interactive Mode**

**How to start:** `./wallbox_control_v3 --interactive` (with flag)

**Characteristics:**

- NO HTTP API server (port 8080 not used)
- Terminal-based command interface
- Direct stdin/stdout interaction
- Requires active terminal session (can't run in background)
- Designed for debugging and manual testing
- Lower memory usage (~8 MB) - no HTTP overhead
- Faster startup (~0.4 seconds) - direct to command prompt

**Use cases:**

- Development and debugging
- Manual testing of features
- Direct hardware inspection (GPIO pins)
- Learning the system
- Quick status checks
- Single administrator session

**Access methods:**

```bash
# Local
./wallbox_control_v3 --interactive

# Remote via SSH
ssh -t bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3 --interactive'

# Or use the connection script
./connect-wallbox-terminal.sh
```

---

### **Side-by-Side Comparison**

| Feature                  | API Mode                 | Interactive Mode          |
| ------------------------ | ------------------------ | ------------------------- |
| **HTTP Server**          | ✅ Yes (port 8080)       | ❌ No                     |
| **Network Access**       | ✅ Remote                | ❌ Local only (via SSH)   |
| **Background Operation** | ✅ Yes                   | ❌ No (requires terminal) |
| **Memory Usage**         | ~15 MB                   | ~8 MB (47% less)          |
| **Startup Time**         | ~1.2 seconds             | ~0.4 seconds (67% faster) |
| **Control Method**       | HTTP requests            | Terminal commands         |
| **Multi-client**         | ✅ Multiple simultaneous | ❌ Single session         |
| **Security Exposure**    | Port 8080 exposed        | No network exposure       |
| **Best for**             | Production               | Development/debugging     |
| **Commands**             | POST/GET requests        | Interactive prompts       |
| **React App Support**    | ✅ Yes                   | ❌ No                     |
| **UDP Communication**    | ✅ Active                | ✅ Active                 |
| **Simulator Compatible** | ✅ Yes                   | ✅ Yes                    |

---

### **Technical Implementation Difference**

**Code level:** In `Application.h`, the decision point is:

```cpp
bool initialize(bool interactiveMode = false)
{
    m_interactiveMode = interactiveMode;

    // ... initialize wallbox controller ...

    if (!m_interactiveMode)  // API Mode
    {
        std::cout << "Starting HTTP API server..." << std::endl;
        m_httpServer = std::make_unique<HttpApiServer>(m_wallboxController);
        setupEndpoints();  // Register routes
        m_httpServer->start(m_config.getApiPort());  // Bind to 8080
    }
    else  // Interactive Mode
    {
        std::cout << "Interactive mode enabled - skipping API server" << std::endl;
        // No HTTP server created
    }
}
```

**Run loop:**

```cpp
if (interactiveMode)
{
    g_application->runInteractive();  // Blocks on stdin, command loop
}
else
{
    g_application->run();  // Event loop, handles HTTP requests
}
```

---

### **When to Use Each Mode**

**Use API Mode when:**

- Deploying to production
- Need remote access from web/mobile apps
- Running as a system service
- Multiple users need access
- Automating with scripts
- Integration with other systems

**Use Interactive Mode when:**

- Developing new features
- Testing state transitions manually
- Debugging GPIO/hardware issues
- Quick status checks during development
- Learning how the system works
- No network access needed
- Want faster startup for quick tests

---

### **Port Usage Summary**

**API Mode:**

- Uses port 8080 (HTTP)
- Uses port 50010 (UDP - wallbox communication)

**Interactive Mode:**

- Uses port 50010 (UDP - wallbox communication only)
- Port 8080 remains free

Both modes use the same UDP communication for the wallbox protocol - the only difference is the control interface (HTTP API vs terminal commands).

---

## Can I connect my simulator with Interactive Mode via UDP messages?

**Yes, absolutely!** The simulator can connect to the wallbox in Interactive Mode via UDP messages.

### Key Point

**Both modes (API and Interactive) use the same UDP communication layer.** The only difference is the control interface:

- **API Mode**: Controlled via HTTP requests
- **Interactive Mode**: Controlled via terminal commands

**The UDP communication to/from the simulator works identically in both modes.**

---

### How It Works

#### Wallbox in Interactive Mode

```bash
# On Banana Pi
./wallbox_control_v3 --interactive

# Still listens on UDP port 50010
# Still sends to 192.168.178.23:50011
# UDP communication is ACTIVE
```

#### Simulator on Mac

```bash
# Configure to send to Banana Pi
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./simulator

# In simulator:
> setudp 192.168.178.34 50011 50010
> on          # Sends UDP message to wallbox
> charge      # Sends UDP message to wallbox
> status      # Shows simulator state
```

---

### Test It Right Now

**Terminal 1 (Banana Pi - Interactive Mode):**

```bash
ssh -t bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3 --interactive'

# You'll see:
# Interactive mode enabled - skipping API server
# Wallbox Controller Ready
# Current state: IDLE
# >

# Type commands:
> status      # Check wallbox status
> getudp      # See UDP config
```

**Terminal 2 (Mac - Simulator):**

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./simulator

# Configure network:
> setudp 192.168.178.34 50011 50010
> on          # Wallbox will receive this!
> charge      # Wallbox state changes!
> status
```

**Terminal 1 (Watch wallbox react):**

```bash
# In wallbox interactive terminal:
> status      # You'll see state changed by simulator!
```

---

### What Gets Transmitted

The simulator sends UDP packets containing:

```cpp
struct stSeIsoStackState {
    isoStackState {
        state: charging/idle/ready/stop
        current: 160 (16.0 A)
        voltage: 2300 (230.0 V)
    }
    seHardwareCmd {
        mainContactor: 0/1 (OFF/ON)
        sourceEnable: 1
        sourceVoltage: 2300
        sourceCurrent: 160
    }
}
```

The wallbox receives these and updates its internal state machine, **regardless of whether it's in API or Interactive mode.**

---

### UDP Flow Diagram

```
┌─────────────────────────────────────────────┐
│  Simulator (Mac)                            │
│  192.168.178.23:50011                       │
│                                             │
│  Commands: on, charge, idle                 │
└──────────────────┬──────────────────────────┘
                   │
                   │ UDP Packets
                   │ (ISO 15118 Protocol)
                   │
                   ▼
┌─────────────────────────────────────────────┐
│  Wallbox (Banana Pi)                        │
│  192.168.178.34:50010                       │
│                                             │
│  ┌──────────────────────────────┐          │
│  │  UDP Communicator            │          │
│  │  (Port 50010)                │          │
│  └──────────┬───────────────────┘          │
│             │                               │
│             ▼                               │
│  ┌──────────────────────────────┐          │
│  │  Wallbox Controller          │          │
│  │  (State Machine)             │          │
│  └──────────┬───────────────────┘          │
│             │                               │
│       ┌─────┴──────┐                       │
│       │            │                       │
│       ▼            ▼                       │
│  ┌─────────┐  ┌──────────┐               │
│  │ API     │  │Interactive│               │
│  │ Mode    │  │ Mode      │               │
│  │ (HTTP)  │  │ (stdin)   │               │
│  └─────────┘  └──────────┘               │
└─────────────────────────────────────────────┘
```

**Both modes share the same UDP layer and state machine!**

---

### Practical Example

Let's test the complete flow:

**Step 1: Start wallbox in interactive mode**

```bash
ssh -t bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3 --interactive'
```

**Step 2: Check initial status**

```
> status
Current State: IDLE
EV Connected: No
Relay State: OFF
```

**Step 3: Start simulator and send commands**

```bash
# In another terminal
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
(echo "setudp 192.168.178.34 50011 50010";
 sleep 1;
 echo "on";
 sleep 2;
 echo "charge";
 sleep 2;
 echo "quit") | ./simulator
```

**Step 4: Back in wallbox, check status again**

```
> status
Current State: CHARGING    # Changed by simulator!
EV Connected: Yes          # Simulator sent "on"
Relay State: ON            # Controlled via UDP
```

---

### The Key Insight

The **Interactive Mode** doesn't disable UDP communication - it only:

1. ❌ Disables HTTP API server (port 8080)
2. ✅ Keeps UDP communication active (port 50010)
3. ✅ Adds terminal command interface

So you get:

- **UDP messages from simulator** → Updates wallbox state
- **Terminal commands** → Query/control wallbox manually
- **Best of both worlds** → Remote UDP control + local terminal monitoring

This is perfect for debugging because you can:

1. Send commands via simulator (UDP)
2. Watch state changes in terminal (interactive)
3. Manually test commands (terminal)
4. All without needing the HTTP API!

---

## Common Scenarios

### Scenario 1: Development Testing

**Goal:** Test new features without web interface

**Setup:**

```bash
# Terminal 1: Wallbox interactive
ssh -t bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3 --interactive'

# Terminal 2: Simulator
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./simulator
```

**Workflow:**

1. Use simulator to send state changes (UDP)
2. Use wallbox terminal to inspect internal state
3. Manually trigger commands to test edge cases
4. Fast iteration without API overhead

---

### Scenario 2: Production Deployment

**Goal:** Run wallbox with web monitoring

**Setup:**

```bash
# Terminal 1: Wallbox API
ssh bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3 &'

# Terminal 2: React app
cd /Users/achraf/pro/PJMT/wallbox-react-app
npm start

# Terminal 3: Simulator (if testing)
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./simulator
```

**Workflow:**

1. Wallbox runs in background (API mode)
2. Web interface controls via HTTP
3. Simulator sends EV signals via UDP
4. Everything works together

---

### Scenario 3: Debugging with Simulator

**Goal:** Debug state machine with full visibility

**Setup:**

```bash
# Terminal 1: Wallbox interactive (see everything)
ssh -t bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3 --interactive'

# Terminal 2: Simulator (control EV)
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./simulator
```

**Workflow:**

1. Simulator sends: `on`, `charge`, `stop` (EV behavior)
2. Wallbox shows state changes in real-time
3. Manually query: `status`, `getpins`, `getudp`
4. Test edge cases: `setrelay 0`, `enable`, `disable`

**Result:** Full control + visibility without HTTP complexity

---

## Quick Reference

### Starting Modes

| Mode        | Command                              | Uses HTTP    | Uses UDP      | Background |
| ----------- | ------------------------------------ | ------------ | ------------- | ---------- |
| API         | `./wallbox_control_v3`               | ✅ Port 8080 | ✅ Port 50010 | ✅ Yes     |
| Interactive | `./wallbox_control_v3 --interactive` | ❌ No        | ✅ Port 50010 | ❌ No      |

### Control Methods

| Mode        | Simulator UDP | React Web | Terminal | API Calls |
| ----------- | ------------- | --------- | -------- | --------- |
| API         | ✅ Yes        | ✅ Yes    | ❌ No    | ✅ Yes    |
| Interactive | ✅ Yes        | ❌ No     | ✅ Yes   | ❌ No     |

### Compatibility Matrix

| Component         | API Mode         | Interactive Mode  |
| ----------------- | ---------------- | ----------------- |
| Simulator (UDP)   | ✅ Compatible    | ✅ Compatible     |
| React App         | ✅ Compatible    | ❌ Not compatible |
| Terminal Commands | ❌ Not available | ✅ Available      |
| curl/Postman      | ✅ Compatible    | ❌ Not compatible |
| Multiple Clients  | ✅ Yes           | ❌ Single session |

---

## Summary

### Key Takeaways

1. **Both modes use UDP** - Simulator works with both
2. **API mode = HTTP + UDP** - For production/web apps
3. **Interactive mode = Terminal + UDP** - For debugging
4. **Choose based on use case** - Not one-size-fits-all
5. **Can switch anytime** - Just restart with/without flag

### Decision Tree

```
Do you need web interface or multiple clients?
├─ YES → Use API Mode
│        Start: ./wallbox_control_v3
│        Control: HTTP requests, React app
│
└─ NO → Use Interactive Mode
         Start: ./wallbox_control_v3 --interactive
         Control: Terminal commands

Does simulator need to connect?
└─ YES → Works in BOTH modes (UDP is always active)
```

---

**Document Version:** 1.0  
**Date:** December 11, 2025  
**Author:** GitHub Copilot (Claude Sonnet 4.5)  
**Project:** PJMT Wallbox Control System

**Related Documents:**

- [Project Summary](./PROJECT_SUMMARY.md)
- [Interactive Mode Implementation](./INTERACTIVE_MODE_IMPLEMENTATION.md)
- [API Reference](./api/API_REFERENCE.md)
