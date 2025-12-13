# Wallbox Configuration Guide

## Configuration File: `config.json`

The `config.json` file allows you to easily configure all aspects of the wallbox controller without modifying the code.

### Location

Place the `config.json` file in the same directory where you run `wallbox_control_v3`:

- `/Users/achraf/pro/PJMT/WallboxCtrl/build/config.json` (for running from build/)
- Or copy it to your working directory

### Configuration Options

#### Mode

```json
"mode": "development"
```

- `"development"` - Uses GPIO stub (simulator), safe for testing
- `"production"` - Uses real Banana Pi GPIO pins (controls hardware!)

#### Network Settings

```json
"network": {
  "udp_listen_port": 50010,    // Port wallbox listens on
  "udp_send_port": 50011,      // Port wallbox sends to (simulator)
  "udp_send_address": "127.0.0.1",  // IP address to send to
  "api_port": 8080             // REST API port for React app
}
```

#### GPIO Pin Configuration

```json
"gpio_pins": {
  "relay_enable": 4,    // Main relay control pin
  "led_green": 17,      // Green LED (ready/charging)
  "led_yellow": 27,     // Yellow LED (waiting)
  "led_red": 22,        // Red LED (error/fault)
  "button": 23          // User button input
}
```

**Banana Pi Pin Mapping:**

- Pin 4 = Physical pin 7
- Pin 17 = Physical pin 11
- Pin 27 = Physical pin 13
- Pin 22 = Physical pin 15
- Pin 23 = Physical pin 16

#### Charging Parameters

```json
"charging": {
  "max_current_amps": 16,     // Maximum charging current
  "voltage": 230,              // Voltage (230V for EU)
  "timeout_seconds": 300       // Charging timeout
}
```

#### Logging

```json
"logging": {
  "level": "info",                          // Log level
  "file": "/tmp/wallbox_v3.log",           // Wallbox log file
  "simulator_file": "/tmp/wallbox_simulator.log"  // Simulator log
}
```

### Environment Variable Override

Environment variables take precedence over config file:

```bash
export WALLBOX_MODE=production        # Override mode
export WALLBOX_API_PORT=9090         # Override API port
export WALLBOX_UDP_LISTEN_PORT=50020 # Override UDP listen port
```

### Example Configurations

#### Development Mode (Default)

```json
{
  "mode": "development",
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "127.0.0.1",
    "api_port": 8080
  },
  "gpio_pins": {
    "relay_enable": 4,
    "led_green": 17,
    "led_yellow": 27,
    "led_red": 22,
    "button": 23
  }
}
```

#### Production Mode (Real Hardware)

```json
{
  "mode": "production",
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "192.168.1.100",
    "api_port": 8080
  },
  "gpio_pins": {
    "relay_enable": 4,
    "led_green": 17,
    "led_yellow": 27,
    "led_red": 22,
    "button": 23
  },
  "charging": {
    "max_current_amps": 32,
    "voltage": 230,
    "timeout_seconds": 600
  }
}
```

#### Custom Pin Configuration

```json
{
  "mode": "development",
  "gpio_pins": {
    "relay_enable": 5, // Changed to pin 5
    "led_green": 6, // Changed to pin 6
    "led_yellow": 13, // Changed to pin 13
    "led_red": 19, // Changed to pin 19
    "button": 26 // Changed to pin 26
  }
}
```

### Usage

1. **Copy config file to build directory:**

   ```bash
   cp /Users/achraf/pro/PJMT/WallboxCtrl/config.json /Users/achraf/pro/PJMT/WallboxCtrl/build/
   ```

2. **Edit config.json with your settings:**

   ```bash
   nano /Users/achraf/pro/PJMT/WallboxCtrl/build/config.json
   ```

3. **Run wallbox (it will automatically load config.json):**
   ```bash
   cd /Users/achraf/pro/PJMT/WallboxCtrl/build
   ./wallbox_control_v3
   ```

### Configuration Loading Order

1. **Default values** (hardcoded in Configuration.h)
2. **config.json file** (if found)
3. **Environment variables** (highest priority)

This allows you to have a base configuration in the file and override specific values with environment variables when needed.

### Troubleshooting

**Config file not found:**

- The program will print a warning and use default values
- Make sure `config.json` is in the same directory where you run the executable

**Invalid JSON:**

- Check for missing commas, quotes, or brackets
- Use a JSON validator: https://jsonlint.com/

**Pins not working:**

- Verify pin numbers match your Banana Pi model
- Check physical connections
- Make sure mode is set to "production" for real hardware

### Quick Pin Test

To test if your pin configuration is correct:

```bash
# In development mode
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
./wallbox_control_v3

# Type these commands:
> enable    # Should activate relay (in logs)
> status    # Check status
```
