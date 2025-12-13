# Quick Start Guide - Wallbox Control System

## Simple Manual Mode (Two Terminals)

### Terminal 1 - Wallbox Controller

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/src
./wallbox_ctrl
```

**Available Commands:**

- `enable` - Enable charging
- `disable` - Disable charging
- `status` - Show current status
- `help` - Show help
- `quit` - Exit

### Terminal 2 - ISO 15118 Simulator

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/src
./simulator
```

**Available Commands:**

- `on` - Turn main contactor ON
- `off` - Turn main contactor OFF
- `idle` - Set state to IDLE
- `ready` - Set state to READY
- `charge` - Set state to CHARGING
- `stop` - Set state to STOP
- `status` - Show status
- `help` - Show help
- `quit` - Exit

## Test Sequence

1. **In Wallbox terminal:**

   ```
   > enable
   > status
   ```

2. **In Simulator terminal:**

   ```
   > on
   > ready
   > charge
   > status
   ```

3. **Watch the interaction - charging should start!**

4. **To stop charging:**
   ```
   Simulator: > stop
   Wallbox: > disable
   ```

## Log Files

All diagnostic logs are written to files (no terminal clutter):

- **Wallbox logs:** `/tmp/wallbox_main.log`
- **Simulator logs:** `/tmp/wallbox_simulator.log`

**View logs in real-time (optional, separate terminal):**

```bash
# View wallbox logs
tail -f /tmp/wallbox_main.log

# View simulator logs
tail -f /tmp/wallbox_simulator.log
```

## Advanced Mode (With React UI)

### Option A: Full Background Mode (Web Control Only)

```bash
bash scripts/start-dev.sh
cd wallbox-react-app && npm start
```

Control everything through the web interface.

### Option B: Interactive Dev Mode (Terminal + Web)

Run these in **separate terminals**:

**Terminal 1 - API Server:**

```bash
cd /Users/achraf/pro/PJMT
bash scripts/start-api-only.sh
```

**Terminal 2 - Interactive Simulator:**

```bash
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./simulator
```

Now you can type commands like: `on`, `ready`, `charge`, `status`

**Terminal 3 - React App:**

```bash
cd /Users/achraf/pro/PJMT/wallbox-react-app
npm start
```

Open http://localhost:3000

**You now have:**

- ✅ Interactive simulator (type commands in terminal)
- ✅ React web interface (control via browser)
- ✅ Both working at the same time!

---

**Terminals are clean - just enter commands and see the program reactions!**
