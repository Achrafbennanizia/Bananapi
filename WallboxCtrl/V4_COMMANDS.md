# Wallbox Controller v4 - Quick Commands Reference

## Running the Controller

### Development Mode (Simulator - for testing on Mac/PC)

```bash
cd <PROJECT_ROOT>/WallboxCtrl/build
export WALLBOX_MODE=dev
./wallbox_control_v4
```

### Production Mode (Real Hardware - Banana Pi only)

```bash
cd <PROJECT_ROOT>/WallboxCtrl/build
export WALLBOX_MODE=prod
./wallbox_control_v4
```

Or use the shortcut:

```bash
cd <PROJECT_ROOT>/WallboxCtrl/build
export WALLBOX_MODE=production
./wallbox_control_v4
```

## On Banana Pi Hardware

When running on the actual Banana Pi, you may need sudo permissions:

```bash
cd WallboxCtrl/build
sudo su
export WALLBOX_MODE=prod
./wallbox_control_v4
```

## Interactive Commands

Once running, use these commands:

| Command  | Description                 |
| -------- | --------------------------- |
| `on`     | Enable relay (GPIO 21 HIGH) |
| `off`    | Disable relay (GPIO 21 LOW) |
| `status` | Show current relay status   |
| `quit`   | Exit program                |
| `exit`   | Exit program                |

## GPIO Pin Configuration

- **v4 Default Pin:** GPIO 21
- **Mode:** Output
- **Control:** Direct digital write (HIGH/LOW)

## Modes Comparison

| Mode                  | GPIO Type        | Hardware       | Use Case            |
| --------------------- | ---------------- | -------------- | ------------------- |
| `dev` / `development` | Stub (Simulator) | No real pins   | Testing on Mac/PC   |
| `prod` / `production` | Banana Pi        | Real GPIO pins | Running on hardware |

## Example Sessions

### Development Mode (Simulator)

```
==================================================
  Wallbox Controller v4.0 - Simple Relay Control
==================================================
Mode: development
GPIO Type: stub
Relay Pin: 21

Initializing Simple Wallbox Controller...
Simple Wallbox Controller initialized successfully

> on
Relay ENABLED on pin 21

> status
Relay status: ON

> off
Relay DISABLED on pin 21

> quit
Goodbye!
```

### Production Mode (Banana Pi)

```
==================================================
  Wallbox Controller v4.0 - Simple Relay Control
==================================================
Mode: production
GPIO Type: bananapi
Relay Pin: 21

[BananaPi GPIO] Using real hardware GPIO via sysfs
Initializing Simple Wallbox Controller...
Simple Wallbox Controller initialized successfully

> on
Relay ENABLED on pin 21
[BananaPi GPIO] Write pin 21 = HIGH

> off
Relay DISABLED on pin 21
[BananaPi GPIO] Write pin 21 = LOW

> quit
Goodbye!
```

## Building

Rebuild if needed:

```bash
cd WallboxCtrl/build
cmake ..
make wallbox_control_v4
```

## Troubleshooting

### "GPIO sysfs not available"

- **Cause:** Running production mode on Mac/PC
- **Solution:** Use `export WALLBOX_MODE=dev` for testing

### "Permission denied" on Banana Pi

- **Cause:** Need root access for GPIO
- **Solution:** Run with `sudo` or as root user

### "File not found"

- **Cause:** Not in build directory
- **Solution:** `cd <PROJECT_ROOT>/WallboxCtrl/build`
