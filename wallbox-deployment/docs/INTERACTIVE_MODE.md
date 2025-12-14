# Interactive Terminal Mode (No React App)

This mode gives you full terminal control of both wallbox and simulator.

## Setup - Two Terminals

### Terminal 1 - Wallbox Controller

```bash
cd <PROJECT_ROOT>/WallboxCtrl/src
./wallbox_ctrl
```

**Available Commands:**

- `enable` - Enable charging
- `disable` - Disable charging
- `status` - Show current status
- `help` - Show help
- `quit` - Exit

**Logs:** `/tmp/wallbox_main.log`

---

### Terminal 2 - Simulator

```bash
cd <PROJECT_ROOT>/WallboxCtrl/src
./simulator
```

**Available Commands:**

- `on` - Turn main contactor ON
- `off` - Turn main contactor OFF
- `ready` - Set state to READY
- `charge` - Set state to CHARGING
- `stop` - Set state to STOP
- `status` - Show status
- `help` - Show help
- `quit` - Exit

**Logs:** `/tmp/wallbox_simulator.log`

---

## Test Sequence

1. **Wallbox terminal:** `enable`
2. **Simulator terminal:** `on`
3. **Simulator terminal:** `ready`
4. **Simulator terminal:** `charge`
5. **Check status in both terminals:** `status`

---

## Note

⚠️ This mode does NOT work with the React app because `wallbox_ctrl` doesn't have an API server.

For React app + Interactive simulator, see QUICK_START.md - Option B.
