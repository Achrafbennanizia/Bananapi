/*
 * Banana Pi M5 Auto-Setup via USB HID Keyboard Emulation
 * For Arduino Leonardo, Pro Micro, or any ATmega32U4 board
 *
 * IMPORTANT: Use Arduino Leonardo or Pro Micro (ATmega32U4)
 * Regular Arduino Uno/Nano do NOT support USB HID!
 */

#include <Keyboard.h>

// ============================================
// CONFIGURATION - EDIT THESE VALUES
// ============================================

// Login credentials
const char *USERNAME = "root";
const char *PASSWORD = "1234";           // Default Armbian password
const char *NEW_PASSWORD = "wallbox123"; // New password to set

// Timing delays (in milliseconds)
const int INITIAL_DELAY = 5000; // Safety delay
const int BOOT_WAIT = 30000;    // Wait for system boot
const int AFTER_LOGIN_DELAY = 2000;
const int AFTER_PASSWORD_DELAY = 3000;
const int COMMAND_DELAY = 1500;
const int CHAR_DELAY = 20; // Typing speed

// LED pin (usually 13 on Leonardo)
const int LED_PIN = LED_BUILTIN;

// ============================================
// COMMANDS TO EXECUTE
// ============================================

const char *commands[] = {
    // Enable SSH permanently
    "systemctl unmask ssh",
    "systemctl enable ssh",
    "systemctl start ssh",

    // Allow root login via SSH
    "sed -i 's/#PermitRootLogin.*/PermitRootLogin yes/' /etc/ssh/sshd_config",
    "sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config",

    // Restart SSH
    "systemctl restart ssh",

    // Show IP address
    "ip addr show | grep 'inet '",

    // Confirm SSH status
    "systemctl status ssh --no-pager | head -5",

    // Success message
    "echo '=== SSH ENABLED SUCCESSFULLY ==='"};

const int NUM_COMMANDS = sizeof(commands) / sizeof(commands[0]);

// ============================================
// HELPER FUNCTIONS
// ============================================

void blinkLED(int times, int interval)
{
    for (int i = 0; i < times; i++)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(interval);
        digitalWrite(LED_PIN, LOW);
        delay(interval);
    }
}

void typeString(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        Keyboard.print(str[i]);
        delay(CHAR_DELAY);
    }
}

void typeLine(const char *str)
{
    typeString(str);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.release(KEY_RETURN);
}

void pressEnter()
{
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.release(KEY_RETURN);
}

void pressCtrlC()
{
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('c');
    delay(100);
    Keyboard.releaseAll();
}

// ============================================
// SETUP - RUNS ONCE
// ============================================

void setup()
{
    pinMode(LED_PIN, OUTPUT);

    // Safety delay with blinking (time to abort)
    Serial.begin(9600);
    Serial.println("USB HID Keyboard starting...");

    for (int i = 5; i > 0; i--)
    {
        Serial.print("Starting in ");
        Serial.println(i);
        blinkLED(1, 500);
    }

    // Fast blink to indicate starting
    blinkLED(5, 100);

    // Initialize keyboard
    Keyboard.begin();

    // Wait for system to boot
    Serial.println("Waiting for system boot...");
    digitalWrite(LED_PIN, HIGH);
    delay(BOOT_WAIT);
    digitalWrite(LED_PIN, LOW);

    // Activate console
    Serial.println("Activating console...");
    pressEnter();
    delay(2000);
    pressEnter();
    delay(1000);

    // Enter username
    Serial.println("Entering username...");
    blinkLED(2, 100);
    typeLine(USERNAME);
    delay(AFTER_LOGIN_DELAY);

    // Enter password
    Serial.println("Entering password...");
    blinkLED(2, 100);
    typeLine(PASSWORD);
    delay(AFTER_PASSWORD_DELAY);

    // Handle Armbian first-login password change
    Serial.println("Handling password change...");
    delay(2000);

    // Current password
    typeLine(PASSWORD);
    delay(1000);

    // New password
    typeLine(NEW_PASSWORD);
    delay(1000);

    // Confirm new password
    typeLine(NEW_PASSWORD);
    delay(2000);

    // Skip prompts
    pressEnter();
    delay(1000);
    pressCtrlC();
    delay(1000);
    pressEnter();
    delay(2000);

    // Execute commands
    Serial.println("Executing commands...");
    digitalWrite(LED_PIN, HIGH);

    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        Serial.print("Command ");
        Serial.print(i + 1);
        Serial.print("/");
        Serial.println(NUM_COMMANDS);

        blinkLED(1, 100);
        typeLine(commands[i]);
        delay(COMMAND_DELAY);
    }

    // Create ensure-ssh service (multi-line command)
    Serial.println("Creating SSH service...");
    typeString("cat > /etc/systemd/system/ensure-ssh.service << 'EOF'\n");
    delay(500);
    typeString("[Unit]\n");
    typeString("Description=Ensure SSH is always enabled\n");
    typeString("After=network.target\n\n");
    typeString("[Service]\n");
    typeString("Type=oneshot\n");
    typeString("ExecStart=/bin/systemctl unmask ssh\n");
    typeString("ExecStart=/bin/systemctl enable ssh\n");
    typeString("ExecStart=/bin/systemctl start ssh\n");
    typeString("RemainAfterExit=yes\n\n");
    typeString("[Install]\n");
    typeString("WantedBy=multi-user.target\n");
    typeLine("EOF");
    delay(1000);

    // Enable the service
    typeLine("systemctl daemon-reload");
    delay(500);
    typeLine("systemctl enable ensure-ssh.service");
    delay(500);

    // Done
    Serial.println("=== CONFIGURATION COMPLETE ===");
    Keyboard.end();

    // Victory blink
    for (int i = 0; i < 3; i++)
    {
        blinkLED(3, 100);
        delay(500);
    }

    // Keep LED on
    digitalWrite(LED_PIN, HIGH);
}

// ============================================
// LOOP - RUNS REPEATEDLY
// ============================================

void loop()
{
    // Nothing to do - just keep LED on
    delay(1000);
}
