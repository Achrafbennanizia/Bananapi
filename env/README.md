# Arduino CLI Development Environment

This project provides an Arduino development environment using Arduino CLI for programming and managing Arduino boards.

## Prerequisites

- macOS (or Linux/Windows with adjustments)
- Arduino CLI installed on your system
- USB cable to connect your Arduino board

## Installation

### Install Arduino CLI

If you haven't installed Arduino CLI yet, use Homebrew:

```bash
brew install arduino-cli
```

### Update Core Index

Before using Arduino CLI, update the package index:

```bash
arduino-cli core update-index
```

## Setup Your Arduino Board

### 1. Detect Connected Boards

Connect your Arduino board via USB and run:

```bash
arduino-cli board list
```

This will show all connected serial devices and detected Arduino boards.

### 2. Install Board Core

Install the appropriate core for your Arduino board:

**For Arduino Uno, Mega, Nano (AVR-based boards):**

```bash
arduino-cli core install arduino:avr
```

**For Arduino MKR, Nano 33 IoT (SAMD-based boards):**

```bash
arduino-cli core install arduino:samd
```

**For ESP32 boards:**

```bash
arduino-cli config init
arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32
```

**For ESP8266 boards:**

```bash
arduino-cli config add board_manager.additional_urls http://arduino.esp8266.com/stable/package_esp8266com_index.json
arduino-cli core update-index
arduino-cli core install esp8266:esp8266
```

### 3. List Installed Cores

Verify your installed cores:

```bash
arduino-cli core list
```

## Working with Sketches

### Create a New Sketch

```bash
arduino-cli sketch new MyProject
cd MyProject
```

This creates a new directory with a basic `.ino` file.

### Compile a Sketch

Replace `arduino:avr:uno` with your board's FQBN (Fully Qualified Board Name):

```bash
arduino-cli compile --fqbn arduino:avr:uno MyProject
```

**Common FQBNs:**

- Arduino Uno: `arduino:avr:uno`
- Arduino Mega: `arduino:avr:mega`
- Arduino Nano: `arduino:avr:nano`
- ESP32: `esp32:esp32:esp32`
- ESP8266: `esp8266:esp8266:generic`

### Upload to Board

Replace `/dev/cu.MAJORIV` with your board's port from `arduino-cli board list`:

```bash
arduino-cli upload -p /dev/cu.MAJORIV --fqbn arduino:avr:uno MyProject
```

### Compile and Upload in One Command

```bash
arduino-cli compile --upload -p /dev/cu.MAJORIV --fqbn arduino:avr:uno MyProject
```

## Managing Libraries

### Search for Libraries

```bash
arduino-cli lib search [library-name]
```

### Install a Library

```bash
arduino-cli lib install "Servo"
arduino-cli lib install "Adafruit NeoPixel"
```

### List Installed Libraries

```bash
arduino-cli lib list
```

### Update All Libraries

```bash
arduino-cli lib upgrade
```

## Useful Commands

### Get Board Details

```bash
arduino-cli board details --fqbn arduino:avr:uno
```

### Monitor Serial Output

```bash
arduino-cli monitor -p /dev/cu.MAJORIV -c baudrate=9600
```

### Search for Boards

```bash
arduino-cli board search
```

### Configuration

View current configuration:

```bash
arduino-cli config dump
```

Initialize configuration file:

```bash
arduino-cli config init
```

## Example Workflow

Here's a complete example workflow for creating and uploading a blink sketch:

```bash
# 1. Create new sketch
arduino-cli sketch new Blink
cd Blink

# 2. Edit Blink.ino (add your code)
# The file is already created with basic structure

# 3. Detect your board
arduino-cli board list

# 4. Install core if needed
arduino-cli core install arduino:avr

# 5. Compile the sketch
arduino-cli compile --fqbn arduino:avr:uno Blink

# 6. Upload to board (replace port with your actual port)
arduino-cli upload -p /dev/cu.MAJORIV --fqbn arduino:avr:uno Blink

# 7. Monitor serial output
arduino-cli monitor -p /dev/cu.MAJORIV -c baudrate=9600
```

## Troubleshooting

### Board Not Detected

- Check USB connection
- Try different USB cable
- Check if board appears in `/dev/cu.*` with: `ls /dev/cu.*`
- On macOS, install CH340 driver if using clone boards

### Permission Denied

- On Linux, add your user to dialout group: `sudo usermod -a -G dialout $USER`
- Log out and back in
- On macOS, usually no permission issues

### Compilation Errors

- Ensure correct core is installed
- Verify FQBN matches your board
- Check library dependencies

## Arduino Simulation (No Physical Hardware Required)

If you want to develop and test Arduino code without a physical board, you have several options:

### Option 1: Wokwi Online Simulator (Recommended)

[Wokwi](https://wokwi.com/) is a free online Arduino and ESP32 simulator with a visual interface.

**Features:**

- Simulates Arduino Uno, Mega, Nano, ESP32, and more
- Visual circuit builder with components (LEDs, sensors, displays, etc.)
- Real-time simulation
- Works in browser, no installation needed
- Supports Serial Monitor and Plotter

**Usage:**

1. Visit [wokwi.com](https://wokwi.com/)
2. Click "Start a new project"
3. Select Arduino board type
4. Write your code in the editor
5. Add components visually
6. Click "Start Simulation"

### Option 2: SimulIDE Desktop Simulator

[SimulIDE](https://simulide.com/) is a free desktop circuit simulator supporting Arduino.

**Installation:**

```bash
# Download from https://simulide.com/p/downloads/
# Or via Homebrew (if available)
brew install --cask simulide
```

**Features:**

- Full circuit simulation
- Arduino, PIC, AVR support
- Component library (LEDs, motors, sensors)
- Real-time debugging
- Offline usage

### Option 3: Virtual Serial Port for Testing

Create virtual serial ports to test compilation and serial communication without hardware:

**Install socat (virtual serial port tool):**

```bash
brew install socat
```

**Create virtual serial ports:**

```bash
# Terminal 1: Create virtual port pair
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# Note the two device paths it creates (e.g., /dev/ttys001 and /dev/ttys002)
```

**Use with Arduino CLI:**

```bash
# Compile your sketch
arduino-cli compile --fqbn arduino:avr:uno MyProject

# The compiled .hex file is in MyProject/build/
# You can inspect it but can't truly "run" it without a simulator
```

### Option 4: Docker-based Development (Compile Only)

Use Docker for a consistent development environment without needing hardware:

```bash
# Build the image
docker-compose build

# Run interactively for compilation and testing
docker-compose run --rm arduino-cli bash

# Inside container - compile sketches
arduino-cli core install arduino:avr
arduino-cli sketch new TestProject
arduino-cli compile --fqbn arduino:avr:uno TestProject
```

**Benefits:**

- Isolated environment
- Consistent across different machines
- Perfect for CI/CD pipelines
- No hardware required for compilation testing

### Option 5: PlatformIO with Simulation

[PlatformIO](https://platformio.org/) supports simulation for some boards.

**Install PlatformIO:**

```bash
brew install platformio
```

**Create and simulate project:**

```bash
# Create new project
pio project init --board uno

# Add simulation dependencies
# Edit platformio.ini to add simulation support
```

### Recommended Workflow for Virtual Development

1. **Write and compile** code using Arduino CLI or Docker
2. **Test logic** using Wokwi or SimulIDE for visual feedback
3. **Debug serial communication** using virtual serial ports
4. **Deploy to real hardware** when ready

### Example: Compile-Only Workflow (No Hardware)

```bash
# 1. Install core
arduino-cli core install arduino:avr

# 2. Create sketch
arduino-cli sketch new BlinkTest
cd BlinkTest

# 3. Edit BlinkTest.ino with your code

# 4. Compile for target board
arduino-cli compile --fqbn arduino:avr:uno BlinkTest

# 5. View build artifacts
ls -la build/arduino.avr.uno/

# 6. Test in Wokwi simulator online
# - Copy your code to wokwi.com
# - Run simulation virtually
```

### Docker Alternative for Hardware

If you have physical hardware but prefer using Docker:

```bash
# Build the image
docker-compose build

# Run interactively with USB device
docker-compose run --rm arduino-cli

# Inside container
arduino-cli core update-index
arduino-cli board list
```

**Note:** Update the device path in `docker-compose.yml` to match your board's port.

## Resources

- [Arduino CLI Documentation](https://arduino.github.io/arduino-cli/)
- [Arduino CLI GitHub](https://github.com/arduino/arduino-cli)
- [Arduino Language Reference](https://www.arduino.cc/reference/en/)

## License

This project is open source and available for educational purposes.
