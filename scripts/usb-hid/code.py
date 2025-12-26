# SPDX-License-Identifier: MIT
# Banana Pi M5 Auto-Setup via USB HID Keyboard Emulation
# For Raspberry Pi Pico with CircuitPython

import time
import board
import digitalio
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS
from adafruit_hid.keycode import Keycode

# ============================================
# CONFIGURATION - EDIT THESE VALUES
# ============================================

# Login credentials for Banana Pi
USERNAME = "root"
PASSWORD = "1234"  # Default Armbian password

# New password to set (Armbian requires password change on first login)
NEW_PASSWORD = "wallbox123"

# Commands to execute after login
COMMANDS = [
    # Enable SSH permanently
    "systemctl unmask ssh",
    "systemctl enable ssh",
    "systemctl start ssh",
    
    # Allow root login via SSH
    "sed -i 's/#PermitRootLogin.*/PermitRootLogin yes/' /etc/ssh/sshd_config",
    "sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config",
    
    # Restart SSH to apply changes
    "systemctl restart ssh",
    
    # Create ensure-ssh service for persistence
    '''cat > /etc/systemd/system/ensure-ssh.service << 'SSHEOF'
[Unit]
Description=Ensure SSH is always enabled
After=network.target

[Service]
Type=oneshot
ExecStart=/bin/systemctl unmask ssh
ExecStart=/bin/systemctl enable ssh
ExecStart=/bin/systemctl start ssh
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
SSHEOF''',
    
    # Enable the ensure-ssh service
    "systemctl daemon-reload",
    "systemctl enable ensure-ssh.service",
    
    # Show IP address
    "ip addr show | grep 'inet '",
    
    # Confirm SSH is running
    "systemctl status ssh --no-pager | head -5",
    
    # Print success message
    "echo '=== SSH ENABLED SUCCESSFULLY ==='",
]

# Timing delays (in seconds)
INITIAL_DELAY = 5       # Wait before starting (safety delay)
BOOT_WAIT = 30          # Wait for system to boot
AFTER_LOGIN_DELAY = 2   # Wait after entering username
AFTER_PASSWORD_DELAY = 3 # Wait after entering password
COMMAND_DELAY = 1       # Wait between commands
CHAR_DELAY = 0.02       # Delay between characters (typing speed)

# ============================================
# LED SETUP
# ============================================

led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT

def blink(times=1, interval=0.2):
    """Blink the LED"""
    for _ in range(times):
        led.value = True
        time.sleep(interval)
        led.value = False
        time.sleep(interval)

def led_on():
    led.value = True

def led_off():
    led.value = False

# ============================================
# KEYBOARD SETUP
# ============================================

# Wait for USB to be ready
time.sleep(1)

keyboard = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(keyboard)

def type_string(text, delay=CHAR_DELAY):
    """Type a string character by character"""
    for char in text:
        layout.write(char)
        time.sleep(delay)

def press_enter():
    """Press Enter key"""
    keyboard.send(Keycode.ENTER)
    time.sleep(0.1)

def press_key(keycode):
    """Press a specific key"""
    keyboard.send(keycode)
    time.sleep(0.1)

def type_line(text, delay=CHAR_DELAY):
    """Type a line and press Enter"""
    type_string(text, delay)
    press_enter()

# ============================================
# MAIN SCRIPT
# ============================================

def main():
    print("USB HID Keyboard Emulator Starting...")
    
    # Safety delay with slow blink (gives time to abort)
    print(f"Waiting {INITIAL_DELAY} seconds before starting...")
    for i in range(INITIAL_DELAY):
        blink(1, 0.5)
        print(f"  {INITIAL_DELAY - i}...")
    
    # Fast blink to indicate starting
    blink(5, 0.1)
    print("Starting keyboard emulation!")
    
    # Wait for system to boot
    print(f"Waiting {BOOT_WAIT} seconds for system boot...")
    led_on()
    time.sleep(BOOT_WAIT)
    led_off()
    
    # Press Enter to activate console
    print("Activating console...")
    press_enter()
    time.sleep(2)
    press_enter()
    time.sleep(1)
    
    # Enter username
    print(f"Entering username: {USERNAME}")
    blink(2, 0.1)
    type_line(USERNAME)
    time.sleep(AFTER_LOGIN_DELAY)
    
    # Enter password
    print("Entering password...")
    blink(2, 0.1)
    type_line(PASSWORD)
    time.sleep(AFTER_PASSWORD_DELAY)
    
    # Handle Armbian first-login password change (if needed)
    # Armbian asks: Current password, New password, Retype new password
    print("Handling potential password change prompt...")
    time.sleep(2)
    
    # Enter current password again (in case asked)
    type_line(PASSWORD)
    time.sleep(1)
    
    # Enter new password
    type_line(NEW_PASSWORD)
    time.sleep(1)
    
    # Confirm new password
    type_line(NEW_PASSWORD)
    time.sleep(2)
    
    # Skip shell selection (if asked) - just press Enter
    press_enter()
    time.sleep(1)
    
    # Skip user creation (if asked) - Ctrl+C to skip
    keyboard.send(Keycode.CONTROL, Keycode.C)
    time.sleep(1)
    press_enter()
    time.sleep(2)
    
    # Now execute commands
    print("Executing configuration commands...")
    led_on()
    
    for i, cmd in enumerate(COMMANDS):
        print(f"Command {i+1}/{len(COMMANDS)}: {cmd[:50]}...")
        blink(1, 0.1)
        
        # Type the command
        type_line(cmd)
        time.sleep(COMMAND_DELAY)
    
    # Final confirmation
    print("Configuration complete!")
    led_on()
    
    # Victory blink pattern
    for _ in range(3):
        blink(3, 0.1)
        time.sleep(0.5)
    
    # Keep LED on to indicate success
    led_on()
    print("=== DONE ===")

# Run the script
if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        # Error blink pattern
        while True:
            blink(10, 0.05)
            time.sleep(1)
