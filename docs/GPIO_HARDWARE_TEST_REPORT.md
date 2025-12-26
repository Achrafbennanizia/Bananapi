# GPIO Hardware Test Report - BananaPi M5

**Date:** December 26, 2025  
**Device:** BananaPi M5 (Armbian 25.5.1)  
**Project:** PJMT Wallbox Controller  
**Author:** Development Team

---

## 📋 Executive Summary

This report documents all hardware tests, code changes, and implementation details for the BananaPi M5 GPIO-based Wallbox controller. All tests have been successfully completed and verified.

---

## 1. GPIO Hardware Configuration

### 1.1 Pin Mapping Overview

| Physical Pin | sysfs GPIO | Function     | Direction | Status    |
| ------------ | ---------- | ------------ | --------- | --------- |
| 21           | 586        | Relay Enable | OUTPUT    | ✅ Tested |
| 22           | 579        | LED Red      | OUTPUT    | ✅ Tested |
| 10           | 590        | LED Yellow   | OUTPUT    | ✅ Tested |
| 24           | 587        | LED Green    | OUTPUT    | ✅ Tested |
| 23           | 588        | Button       | INPUT     | ✅ Tested |
| 19           | 585        | CP Signal    | INPUT     | ✅ Tested |

### 1.2 GPIO Chip Information

```
Chip: gpiochip512 (periphs-banks)
  Base: 512
  Count: 85 pins (512-596)

Chip: gpiochip597 (aobus-banks)
  Base: 597
  Count: 15 pins (597-611)
```

### 1.3 Power Pins Reference

| Pin Numbers                  | Function |
| ---------------------------- | -------- |
| 1, 17                        | 3.3V     |
| 2, 4                         | 5V       |
| 6, 9, 14, 20, 25, 30, 34, 39 | GND      |

---

## 2. LED/Relay Behavior Specification

### 2.1 LED Meaning

| LED        | Indication                                                  |
| ---------- | ----------------------------------------------------------- |
| **Yellow** | Wallbox is enabled                                          |
| **Green**  | Car connected (BLINK in CONNECTED/READY, SOLID in CHARGING) |
| **Red**    | Wallbox is disabled or error state                          |
| **Relay**  | Always ON when wallbox enabled, OFF only when disabled      |

### 2.2 State Matrix

| State                       | Yellow (590) | Green (587)    | Red (579) | Relay (586) |
| --------------------------- | ------------ | -------------- | --------- | ----------- |
| **DISABLED**                | OFF          | OFF            | **ON**    | **OFF**     |
| **IDLE** (enabled, no car)  | **ON**       | OFF            | OFF       | **ON**      |
| **CONNECTED** (car plugged) | **ON**       | **BLINK**      | OFF       | **ON**      |
| **READY** (ready to charge) | **ON**       | **BLINK**      | OFF       | **ON**      |
| **CHARGING** (active)       | **ON**       | **ON (solid)** | OFF       | **ON**      |
| **ERROR**                   | OFF          | OFF            | **ON**    | ON          |

---

## 3. GPIO Test Commands

### 3.1 Check GPIO Chip Details

```bash
# List available GPIO chips
ls /sys/class/gpio/

# Get chip details
for chip in /sys/class/gpio/gpiochip*; do
  echo "Chip: $chip"
  cat $chip/label
  echo "Base: $(cat $chip/base)"
  echo "Ngpio: $(cat $chip/ngpio)"
  echo '---'
done
```

### 3.2 Export and Configure Pins

```bash
# Export a single pin (example: GPIO 579)
sudo bash -c 'echo 579 > /sys/class/gpio/export'

# Set pin as output
sudo bash -c 'echo out > /sys/class/gpio/gpio579/direction'

# Set pin as input (for buttons)
sudo bash -c 'echo in > /sys/class/gpio/gpio588/direction'
```

### 3.3 Turn Single Pin ON/OFF

```bash
# Turn GPIO 579 (Red LED) ON
sudo bash -c 'echo 579 > /sys/class/gpio/export 2>/dev/null; echo out > /sys/class/gpio/gpio579/direction; echo 1 > /sys/class/gpio/gpio579/value && echo "GPIO 579: ON"'

# Turn GPIO 579 (Red LED) OFF
sudo bash -c 'echo 0 > /sys/class/gpio/gpio579/value && echo "GPIO 579: OFF"'

# Turn GPIO 586 (Relay) ON
sudo bash -c 'echo 586 > /sys/class/gpio/export 2>/dev/null; echo out > /sys/class/gpio/gpio586/direction; echo 1 > /sys/class/gpio/gpio586/value && echo "GPIO 586: ON"'

# Turn GPIO 586 (Relay) OFF
sudo bash -c 'echo 0 > /sys/class/gpio/gpio586/value && echo "GPIO 586: OFF"'
```

### 3.4 Turn Multiple Pins ON/OFF

```bash
# Turn all LEDs + relay ON
sudo bash -c 'for pin in 586 579 590 587; do echo $pin > /sys/class/gpio/export 2>/dev/null; echo out > /sys/class/gpio/gpio$pin/direction; echo 1 > /sys/class/gpio/gpio$pin/value; done && echo "All pins: ON"'

# Turn all pins OFF
sudo bash -c 'for pin in 586 579 590 587; do echo 0 > /sys/class/gpio/gpio$pin/value; done && echo "All pins: OFF"'
```

### 3.5 Unexport All Pins

```bash
sudo bash -c 'for pin in 586 579 590 587 588; do echo $pin > /sys/class/gpio/unexport 2>/dev/null; done && echo "All pins unexported"'
```

### 3.6 Blink Test

```bash
# Blink Red LED 3 times
sudo bash -c 'for i in 1 2 3; do echo 1 > /sys/class/gpio/gpio579/value; sleep 0.5; echo 0 > /sys/class/gpio/gpio579/value; sleep 0.5; done'
```

---

## 4. Button Test Commands

### 4.1 Setup Button as Input

```bash
sudo bash -c 'echo 588 > /sys/class/gpio/export 2>/dev/null; echo in > /sys/class/gpio/gpio588/direction; echo "Button (GPIO 588) ready"'
```

### 4.2 Read Button State

```bash
# Single read
cat /sys/class/gpio/gpio588/value

# Continuous monitoring (10 seconds)
sudo bash -c 'for i in $(seq 1 20); do echo "Button: $(cat /sys/class/gpio/gpio588/value)"; sleep 0.5; done'

# Continuous monitoring (Ctrl+C to stop)
while true; do echo "Button: $(cat /sys/class/gpio/gpio588/value)"; sleep 0.3; done
```

### 4.3 Button Behavior

| Value | State                      |
| ----- | -------------------------- |
| `1`   | Not pressed (pulled HIGH)  |
| `0`   | Pressed (connected to GND) |

---

## 5. SSH Remote Commands

### 5.1 Connect to BananaPi

```bash
# Using hostname
ssh bananapi-m5

# Using IP address
ssh root@192.168.178.33
```

### 5.2 Remote GPIO Control

```bash
# Turn single pin ON remotely
ssh bananapi-m5 "sudo bash -c 'echo 579 > /sys/class/gpio/export 2>/dev/null; echo out > /sys/class/gpio/gpio579/direction; echo 1 > /sys/class/gpio/gpio579/value'"

# Turn multiple pins ON remotely
ssh bananapi-m5 "sudo bash -c 'for pin in 586 579 590 587; do echo \$pin > /sys/class/gpio/export 2>/dev/null; echo out > /sys/class/gpio/gpio\$pin/direction; echo 1 > /sys/class/gpio/gpio\$pin/value; done'"

# Turn all pins OFF remotely
ssh bananapi-m5 "sudo bash -c 'for pin in 586 579 590 587; do echo 0 > /sys/class/gpio/gpio\$pin/value; done'"

# Read GPIO states remotely
ssh bananapi-m5 'for pin in 586 587 590 579; do echo -n "Pin $pin: "; cat /sys/class/gpio/gpio$pin/value; done'
```

---

## 6. Build and Deploy Commands

### 6.1 Sync Source Files

```bash
# Sync source files to BananaPi
rsync -avz /Users/achraf/pro/PJMT/src/ bananapi-m5:/root/wallbox-src/src/

# Sync header files to BananaPi
rsync -avz /Users/achraf/pro/PJMT/include/ bananapi-m5:/root/wallbox-src/include/

# Sync config files to BananaPi
rsync -avz /Users/achraf/pro/PJMT/config/ bananapi-m5:/root/wallbox-src/config/

# Copy config to build directory
ssh bananapi-m5 'mkdir -p /root/wallbox-src/build/config && cp /root/wallbox-src/config/*.json /root/wallbox-src/build/config/'
```

### 6.2 Build Application

```bash
# Build on BananaPi
ssh bananapi-m5 'cd /root/wallbox-src/build && make wallbox_control_v4 -j4'
```

### 6.3 Start Application

```bash
# Kill any existing process
ssh bananapi-m5 'pkill -9 -f wallbox_control'

# Start in production mode
ssh bananapi-m5 'cd /root/wallbox-src/build && nohup ./bin/wallbox_control_v4 production </dev/null > /tmp/wallbox.log 2>&1 &'

# Check startup log
ssh bananapi-m5 'head -80 /tmp/wallbox.log'
```

---

## 7. REST API Test Commands

### 7.1 Status Endpoints

```bash
# Check wallbox status
curl -s http://192.168.178.33:8080/api/status | jq .

# Check relay state
curl -s http://192.168.178.33:8080/api/relay | jq .

# Health check
curl -s http://192.168.178.33:8080/health
```

### 7.2 Control Endpoints

```bash
# Enable wallbox (Yellow ON, Relay ON)
curl -s -X POST http://192.168.178.33:8080/api/wallbox/enable | jq .

# Disable wallbox (Red ON, Relay OFF)
curl -s -X POST http://192.168.178.33:8080/api/wallbox/disable | jq .

# Start charging
curl -s -X POST http://192.168.178.33:8080/api/charging/start | jq .

# Stop charging
curl -s -X POST http://192.168.178.33:8080/api/charging/stop | jq .
```

### 7.3 GPIO State Verification

```bash
# Expected output when ENABLED (IDLE):
# Pin 586: 1  (Relay ON)
# Pin 587: 0  (Green OFF - no car)
# Pin 590: 1  (Yellow ON - enabled)
# Pin 579: 0  (Red OFF)

# Expected output when DISABLED:
# Pin 586: 0  (Relay OFF)
# Pin 587: 0  (Green OFF)
# Pin 590: 0  (Yellow OFF)
# Pin 579: 1  (Red ON)
```

### 7.4 Combined Test Script

```bash
# Full enable/disable test with GPIO verification
curl -s -X POST http://192.168.178.33:8080/api/wallbox/enable | jq . && \
sleep 1 && \
echo "=== GPIO States (Enabled) ===" && \
ssh bananapi-m5 'for pin in 586 587 590 579; do echo -n "Pin $pin: "; cat /sys/class/gpio/gpio$pin/value; done' && \
echo "" && \
curl -s -X POST http://192.168.178.33:8080/api/wallbox/disable | jq . && \
sleep 1 && \
echo "=== GPIO States (Disabled) ===" && \
ssh bananapi-m5 'for pin in 586 587 590 579; do echo -n "Pin $pin: "; cat /sys/class/gpio/gpio$pin/value; done'
```

---

## 8. Code Changes Documentation

### 8.1 Configuration.h

**File:** `include/wallbox/Configuration.h`

**Change 1:** Added config file path parameter to `initialize()`

```cpp
// Before
bool initialize(bool interactiveMode = false, bool dualMode = false)

// After
bool initialize(bool interactiveMode = false, bool dualMode = false,
                const std::string &configFile = "config/development.json")
```

**Change 2:** Fixed `loadFromEnvironment()` to preserve config file mode

```cpp
// Before - Always defaulted to development
const char *modeEnv = std::getenv("WALLBOX_MODE");
std::string modeStr = modeEnv ? std::string(modeEnv) : "dev";
m_mode = (modeStr == "prod" || modeStr == "production")
             ? Mode::PRODUCTION
             : Mode::DEVELOPMENT;

// After - Only override if explicitly set
const char *modeEnv = std::getenv("WALLBOX_MODE");
if (modeEnv)
{
    std::string modeStr(modeEnv);
    m_mode = (modeStr == "prod" || modeStr == "production")
                 ? Mode::PRODUCTION
                 : Mode::DEVELOPMENT;
}
```

### 8.2 main_v4.cpp

**File:** `src/core/main_v4.cpp`

**Change:** Added config file argument parsing

```cpp
std::string configFile = "config/development.json"; // Default

for (int i = 1; i < argc; ++i)
{
    std::string arg(argv[i]);
    if (arg == "production" || arg == "prod")
        configFile = "config/production.json";
    else if (arg == "development" || arg == "dev")
        configFile = "config/development.json";
    else if (arg == "test")
        configFile = "config/test.json";
    else if (arg.find(".json") != std::string::npos)
        configFile = arg;
}
```

### 8.3 WallboxController.cpp

**File:** `src/core/WallboxController.cpp`

**Change 1:** `setupGpio()` - Relay ON by default

```cpp
void WallboxController::setupGpio()
{
    auto &config = Configuration::getInstance();

    // Configure output pins
    m_gpio->setPinMode(config.getRelayPin(), PinMode::OUTPUT);
    m_gpio->setPinMode(config.getLedGreenPin(), PinMode::OUTPUT);
    m_gpio->setPinMode(config.getLedYellowPin(), PinMode::OUTPUT);
    m_gpio->setPinMode(config.getLedRedPin(), PinMode::OUTPUT);
    m_gpio->setPinMode(config.getButtonPin(), PinMode::INPUT);

    // Initialize LEDs to OFF
    m_gpio->digitalWrite(config.getLedGreenPin(), PinValue::LOW);
    m_gpio->digitalWrite(config.getLedYellowPin(), PinValue::LOW);
    m_gpio->digitalWrite(config.getLedRedPin(), PinValue::LOW);

    // Relay ON by default when wallbox is enabled
    m_gpio->digitalWrite(config.getRelayPin(), m_wallboxEnabled ? PinValue::HIGH : PinValue::LOW);
    m_relayEnabled = m_wallboxEnabled;
}
```

**Change 2:** `enableWallbox()` - Turn relay ON

```cpp
bool WallboxController::enableWallbox()
{
    m_wallboxEnabled = true;
    std::cout << "\n[WALLBOX] 🟢 Wallbox ENABLED - Relay ON by default" << std::endl;
    setRelayState(true);
    updateLeds();
    return true;
}
```

**Change 3:** `disableWallbox()` - Turn relay OFF

```cpp
bool WallboxController::disableWallbox()
{
    if (m_stateMachine->isCharging())
        stopCharging();

    m_wallboxEnabled = false;
    setRelayState(false);
    std::cout << "\n[WALLBOX] 🔴 Wallbox DISABLED - Relay OFF" << std::endl;
    updateLeds();
    return true;
}
```

**Change 4:** `updateLeds()` - New LED logic

```cpp
void WallboxController::updateLeds()
{
    auto &config = Configuration::getInstance();

    if (!m_wallboxEnabled)
    {
        setLedState(config.getLedGreenPin(), false);
        setLedState(config.getLedYellowPin(), false);
        setLedState(config.getLedRedPin(), true);
        setRelayState(false);
        return;
    }

    setLedState(config.getLedYellowPin(), true);
    setLedState(config.getLedRedPin(), false);

    if (!m_relayEnabled)
        setRelayState(true);

    switch (m_stateMachine->getCurrentState())
    {
        case ChargingState::IDLE:
            showIdleLeds();
            break;
        case ChargingState::CONNECTED:
        case ChargingState::IDENTIFICATION:
            showConnectedLeds();
            break;
        case ChargingState::READY:
            showReadyLeds();
            break;
        case ChargingState::CHARGING:
            showChargingLeds();
            break;
        case ChargingState::ERROR:
            showErrorLeds();
            break;
    }
}
```

**Change 5:** LED state functions

```cpp
void WallboxController::showIdleLeds()
{
    auto &config = Configuration::getInstance();
    setLedState(config.getLedGreenPin(), false);
    setLedState(config.getLedYellowPin(), true);
    setLedState(config.getLedRedPin(), false);
}

void WallboxController::showConnectedLeds()
{
    auto &config = Configuration::getInstance();
    static bool greenState = false;
    greenState = !greenState;
    setLedState(config.getLedGreenPin(), greenState);  // BLINK
    setLedState(config.getLedYellowPin(), true);
    setLedState(config.getLedRedPin(), false);
}

void WallboxController::showReadyLeds()
{
    auto &config = Configuration::getInstance();
    static bool greenState = false;
    greenState = !greenState;
    setLedState(config.getLedGreenPin(), greenState);  // BLINK
    setLedState(config.getLedYellowPin(), true);
    setLedState(config.getLedRedPin(), false);
}

void WallboxController::showChargingLeds()
{
    auto &config = Configuration::getInstance();
    setLedState(config.getLedGreenPin(), true);  // SOLID
    setLedState(config.getLedYellowPin(), true);
    setLedState(config.getLedRedPin(), false);
}

void WallboxController::showErrorLeds()
{
    auto &config = Configuration::getInstance();
    setLedState(config.getLedGreenPin(), false);
    setLedState(config.getLedYellowPin(), false);
    setLedState(config.getLedRedPin(), true);
}
```

### 8.4 Header Files Updated

**Files:**

- `include/wallbox/WallboxController.h`
- `wallbox-portable-deploy/include/WallboxController.h`
- `WallboxCtrl/include/WallboxController.h`

**Change:** Added `showConnectedLeds()` declaration

```cpp
void setLedState(int pin, bool on);
void showIdleLeds();
void showConnectedLeds();  // NEW
void showReadyLeds();
void showChargingLeds();
void showErrorLeds();
void showPausedLeds();
```

---

## 9. Files Modified Summary

| File                                                  | Changes                              |
| ----------------------------------------------------- | ------------------------------------ |
| `src/core/main_v4.cpp`                                | Config file argument parsing         |
| `include/wallbox/Application.h`                       | configFile parameter to initialize() |
| `include/wallbox/Configuration.h`                     | Fixed loadFromEnvironment() override |
| `src/core/WallboxController.cpp`                      | New LED logic, relay default ON      |
| `wallbox-portable-deploy/src/WallboxController.cpp`   | Same LED/relay changes               |
| `WallboxCtrl/src/WallboxController.cpp`               | Same LED/relay changes               |
| `include/wallbox/WallboxController.h`                 | Added showConnectedLeds()            |
| `wallbox-portable-deploy/include/WallboxController.h` | Added showConnectedLeds()            |
| `WallboxCtrl/include/WallboxController.h`             | Added showConnectedLeds()            |
| `config/production.json`                              | BananaPi M5 sysfs GPIO numbers       |

---

## 10. Configuration File

### 10.1 production.json

```json
{
  "mode": "production",
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "127.0.0.1",
    "api_port": 8080
  },
  "gpio_pins": {
    "relay_enable": 586,
    "led_green": 587,
    "led_yellow": 590,
    "led_red": 579,
    "button": 588,
    "cp_pin": 585
  },
  "charging": {
    "max_current_amps": 16,
    "voltage": 230,
    "timeout_seconds": 300
  },
  "logging": {
    "level": "info",
    "file": "/tmp/wallbox_v3.log",
    "simulator_file": "/tmp/wallbox_simulator.log"
  }
}
```

---

## 11. Test Results

### 11.1 Enable Test

```bash
$ curl -s -X POST http://192.168.178.33:8080/api/wallbox/enable | jq .
{
  "success": true,
  "message": true,
  "enabled": true
}

$ ssh bananapi-m5 'for pin in 586 587 590 579; do echo -n "Pin $pin: "; cat /sys/class/gpio/gpio$pin/value; done'
Pin 586: 1  ✅ Relay ON
Pin 587: 0  ✅ Green OFF (no car)
Pin 590: 1  ✅ Yellow ON (enabled)
Pin 579: 0  ✅ Red OFF
```

### 11.2 Disable Test

```bash
$ curl -s -X POST http://192.168.178.33:8080/api/wallbox/disable | jq .
{
  "success": true,
  "message": true,
  "enabled": false
}

$ ssh bananapi-m5 'for pin in 586 587 590 579; do echo -n "Pin $pin: "; cat /sys/class/gpio/gpio$pin/value; done'
Pin 586: 0  ✅ Relay OFF
Pin 587: 0  ✅ Green OFF
Pin 590: 0  ✅ Yellow OFF
Pin 579: 1  ✅ Red ON
```

### 11.3 Status Test

```bash
$ curl -s http://192.168.178.33:8080/api/status | jq .
{
  "state": "IDLE",
  "wallboxEnabled": true,
  "relayEnabled": true,
  "charging": false,
  "timestamp": 1766773855
}
```

---

## 12. Hardware Checklist

- [x] GPIO 586 (Pin 21) - Relay Enable - ✅ Working
- [x] GPIO 579 (Pin 22) - LED Red - ✅ Working
- [x] GPIO 590 (Pin 10) - LED Yellow - ✅ Working
- [x] GPIO 587 (Pin 24) - LED Green - ✅ Working
- [x] GPIO 588 (Pin 23) - Button Input - ✅ Working
- [x] GPIO 585 (Pin 19) - CP Signal - ✅ Configured

---

## 13. Troubleshooting

### 13.1 GPIO Pin Not Found

```bash
# Error: "Datei oder Verzeichnis nicht gefunden"
# Solution: Export the pin first
sudo bash -c 'echo <gpio_number> > /sys/class/gpio/export'
```

### 13.2 SSH Connection Failed

```bash
# Check device is powered on
ping 192.168.178.33

# Check SSH service
nc -zv 192.168.178.33 22
```

### 13.3 Port Already in Use

```bash
# Check what's using port 8080
ssh bananapi-m5 'netstat -tlnp | grep 8080'

# Kill existing process
ssh bananapi-m5 'pkill -9 -f wallbox_control'
```

---

**Report Status:** ✅ All tests passed successfully!

**Last Updated:** December 26, 2025
