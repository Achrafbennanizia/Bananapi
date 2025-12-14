# Complete Synchronization Test Guide

## Setup

**Terminal 1 - Simulator:**

```bash
cd <PROJECT_ROOT>/WallboxCtrl/build
./simulator
```

**Terminal 2 - Wallbox:**

```bash
cd <PROJECT_ROOT>/WallboxCtrl/build
./wallbox_control_v3
```

## Test 1: Wallbox → Simulator Synchronization

### Test 1.1: Wallbox Starts Charging

```
Wallbox Terminal> start
```

**Expected in Simulator:**

```
[WALLBOX → SIMULATOR] 🔌 Relay state changed: OFF → ✅ ON
[WALLBOX → SIMULATOR] 🔄 Auto-sync state: idle → charging
[SIMULATOR] ✓ State synchronized to match wallbox
```

**Verify:** Type `status` in simulator - should show "charging"

### Test 1.2: Wallbox Pauses Charging

```
Wallbox Terminal> pause
```

**Expected in Simulator:**

```
[WALLBOX → SIMULATOR] 🔄 Auto-sync state: charging → ready
[SIMULATOR] ✓ State synchronized to match wallbox
```

**Verify:** Type `status` in simulator - should show "ready"

### Test 1.3: Wallbox Stops Charging

```
Wallbox Terminal> stop
```

**Expected in Simulator:**

```
[WALLBOX → SIMULATOR] 🔌 Relay state changed: ON → ❌ OFF
[WALLBOX → SIMULATOR] 🔄 Auto-sync state: ready → idle
[SIMULATOR] ✓ State synchronized to match wallbox
```

**Verify:** Type `status` in simulator - should show "idle"

## Test 2: Simulator → Wallbox Synchronization

### Test 2.1: Simulator Signals Ready

```
Simulator Terminal> ready
```

**Expected in Wallbox:**

```
[SIMULATOR → WALLBOX] State: idle → ready  (Vehicle ready - can start charging)
```

**Verify:** The wallbox recognizes vehicle is ready

### Test 2.2: Simulator Initiates Charging

```
Simulator Terminal> charge
```

**Expected in Wallbox:**

```
[SIMULATOR → WALLBOX] State: ready → charging
[WALLBOX] 🔄 Auto-sync: Starting charging (simulator → charging)
[WALLBOX → SIMULATOR] Starting charging sequence
```

**Expected in Simulator:**

```
[WALLBOX → SIMULATOR] 🔌 Relay state changed: OFF → ✅ ON
```

**Verify:** Type `status` in wallbox - should show "CHARGING"

### Test 2.3: Simulator Stops Charging

```
Simulator Terminal> stop
```

**Expected in Wallbox:**

```
[SIMULATOR → WALLBOX] State: charging → stop
[WALLBOX] 🔄 Auto-sync: Stopping (simulator → stop)
```

**Expected in Simulator:**

```
[WALLBOX → SIMULATOR] 🔌 Relay state changed: ON → ❌ OFF
[WALLBOX → SIMULATOR] 🔄 Auto-sync state: stop → idle
```

**Verify:** Both should be in idle state

## Test 3: Enable/Disable Synchronization

### Test 3.1: Disable Wallbox

```
Wallbox Terminal> disable
```

**Expected in Simulator:**

```
[WALLBOX → SIMULATOR] 🔴 WALLBOX DISABLED - Charging blocked
```

**Verify:** Simulator commands should be rejected

```
Simulator Terminal> charge
⚠️  Cannot execute 'charge': Wallbox enable=false (charging disabled)
```

### Test 3.2: Re-enable Wallbox

```
Wallbox Terminal> enable
```

**Expected in Simulator:**

```
[WALLBOX → SIMULATOR] 🟢 WALLBOX ENABLED - Ready to charge
```

**Verify:** Simulator commands should work again

## Test 4: Query Status

### Test 4.1: Check Simulator Status

```
Simulator Terminal> status
```

Should show current simulator state (contactor, charging state, UDP config)

### Test 4.2: Check Wallbox Status from Simulator

```
Simulator Terminal> getwb
```

Should show:

- Wallbox enabled/disabled
- Relay state
- Current demand with interpretation
- Decoded wallbox state
- Communication status

### Test 4.3: Check Wallbox Status

```
Wallbox Terminal> status
```

Should show current wallbox state and relay status

## Expected Synchronization Behavior

### ✅ Working Correctly:

1. When wallbox starts → simulator state changes to "charging"
2. When wallbox stops → simulator state changes to "idle"
3. When simulator charges → wallbox starts charging
4. When simulator stops → wallbox stops charging
5. Relay changes propagate immediately
6. Enable/disable status synchronized
7. Console shows sync messages with 🔄 icon

### ⚠️ Check These:

- Do you see "🔄 Auto-sync" messages?
- Does `status` command show same state in both?
- Does `getwb` show correct wallbox state?
- Are relay changes visible in both consoles?

## Troubleshooting

### Problem: No sync messages appear

**Solution:**

1. Check both programs are running
2. Verify UDP ports (50010/50011) are not blocked
3. Check logs: `tail -f /tmp/wallbox_v3.log /tmp/wallbox_simulator.log`

### Problem: Sync only works one direction

**Solution:**

1. Restart both programs
2. Verify initialization messages appear
3. Test with simple commands (start/stop)

### Problem: States don't match

**Solution:**

1. Use `status` in both to check current state
2. Use `getwb` in simulator to see what wallbox is sending
3. Stop both and restart in correct order (simulator first)

## Success Indicators

✅ Console shows "🔄 Auto-sync" messages in both directions  
✅ `status` command shows same state in both programs  
✅ Commands in either program affect the other  
✅ Relay changes appear in both consoles  
✅ Enable/disable blocks/allows commands appropriately  
✅ `getwb` command shows accurate wallbox state

## Log Files

For detailed debugging:

```bash
# Watch both logs
tail -f /tmp/wallbox_v3.log /tmp/wallbox_simulator.log

# Search for sync events
grep "Auto-sync" /tmp/wallbox_v3.log /tmp/wallbox_simulator.log

# Check communication health
grep "DEBUG.*Status received" /tmp/wallbox_simulator.log | tail -5
```

---

**If synchronization is working correctly, you should see:**

- 🔄 icons indicating automatic synchronization
- ✓ confirmation messages
- State changes in both terminals
- Matching states when using `status` command
