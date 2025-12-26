# USB HID Keyboard Emulator for Banana Pi Setup

This script makes a Raspberry Pi Pico act as a USB keyboard to automatically
configure the Banana Pi M5 (enable SSH, configure services, etc.)

## Requirements

- Raspberry Pi Pico ($4-5)
- USB cable (micro-USB for Pico)

## Setup Instructions

### 1. Install CircuitPython on Pico

1. Download CircuitPython for Pico:
   https://circuitpython.org/board/raspberry_pi_pico/

2. Hold BOOTSEL button on Pico while connecting to computer
3. Drag the .uf2 file to the RPI-RP2 drive that appears
4. Pico will reboot and show as "CIRCUITPY" drive

### 2. Install HID Library

1. Download Adafruit HID library:
   https://github.com/adafruit/Adafruit_CircuitPython_HID/releases

2. Copy `adafruit_hid` folder to `CIRCUITPY/lib/`

### 3. Copy the Script

1. Copy `code.py` to the root of CIRCUITPY drive
2. The script will run automatically when Pico is connected

## Usage

1. Connect Pico to Banana Pi's USB port
2. Wait for the LED to blink (indicates script is running)
3. The script will:
   - Wait for login prompt
   - Enter root credentials
   - Enable SSH
   - Configure the system
4. LED will turn solid when complete

## Customization

Edit `code.py` to change:

- Login credentials (default: root/1234)
- Commands to execute
- Timing delays

## Safety

- The script has a 5-second delay before starting (LED blinks slowly)
- Press and hold the button on Pico to abort
- Unplug immediately if something goes wrong
