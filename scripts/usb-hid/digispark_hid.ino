/*
 * Banana Pi M5 SSH Enabler - Digispark ATtiny85 Version
 *
 * For Digispark ATtiny85 boards (~$2)
 *
 * Setup:
 * 1. Install Digispark board in Arduino IDE:
 *    - Add to Additional Boards Manager URLs:
 *      http://digistump.com/package_digistump_index.json
 *    - Install "Digistump AVR Boards"
 *
 * 2. Select board: "Digispark (Default - 16.5mhz)"
 *
 * 3. Upload (plug in Digispark when prompted)
 */

#include "DigiKeyboard.h"

void setup()
{
    pinMode(1, OUTPUT); // LED on Model B

    // Wait for USB enumeration
    DigiKeyboard.delay(5000);

    // Blink to show starting
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(1, HIGH);
        DigiKeyboard.delay(100);
        digitalWrite(1, LOW);
        DigiKeyboard.delay(100);
    }

    // Wait for system to boot
    DigiKeyboard.delay(30000);

    // Activate console
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboard.delay(2000);
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboard.delay(1000);

    // Login: root
    DigiKeyboard.println("root");
    DigiKeyboard.delay(2000);

    // Password: 1234
    DigiKeyboard.println("1234");
    DigiKeyboard.delay(3000);

    // Handle password change (Armbian first boot)
    DigiKeyboard.println("1234"); // Current password
    DigiKeyboard.delay(1000);
    DigiKeyboard.println("wallbox123"); // New password
    DigiKeyboard.delay(1000);
    DigiKeyboard.println("wallbox123"); // Confirm
    DigiKeyboard.delay(2000);

    // Skip prompts
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboard.delay(1000);
    DigiKeyboard.sendKeyStroke(KEY_C, MOD_CONTROL_LEFT); // Ctrl+C
    DigiKeyboard.delay(1000);
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboard.delay(2000);

    // Enable SSH
    DigiKeyboard.println("systemctl unmask ssh");
    DigiKeyboard.delay(1000);
    DigiKeyboard.println("systemctl enable ssh");
    DigiKeyboard.delay(1000);
    DigiKeyboard.println("systemctl start ssh");
    DigiKeyboard.delay(1000);

    // Allow root login
    DigiKeyboard.println("sed -i 's/#PermitRootLogin.*/PermitRootLogin yes/' /etc/ssh/sshd_config");
    DigiKeyboard.delay(1000);
    DigiKeyboard.println("sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config");
    DigiKeyboard.delay(1000);

    // Restart SSH
    DigiKeyboard.println("systemctl restart ssh");
    DigiKeyboard.delay(1000);

    // Show IP and status
    DigiKeyboard.println("echo '=== SSH ENABLED ==='");
    DigiKeyboard.delay(500);
    DigiKeyboard.println("ip addr show | grep 'inet '");
    DigiKeyboard.delay(500);

    // LED on = done
    digitalWrite(1, HIGH);
}

void loop()
{
    // Done - just keep LED on
    DigiKeyboard.delay(1000);
}
