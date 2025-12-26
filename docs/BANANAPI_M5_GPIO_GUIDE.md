# BananaPi M5 GPIO Connection Guide

**Device:** BananaPi M5 (BPI-M5)  
**OS:** Armbian 25.5.1 (Debian 12 Bookworm)  
**GPIO Access:** sysfs (`/sys/class/gpio`)

---

## GPIO Chip Information

| Chip        | Label         | Base | Count | Range   |
| ----------- | ------------- | ---- | ----- | ------- |
| gpiochip512 | periphs-banks | 512  | 85    | 512-596 |
| gpiochip597 | aobus-banks   | 597  | 15    | 597-611 |

---

## 40-Pin Header Pinout (CON2)

```
                    BananaPi M5 - 40 Pin Header
                    ===========================
                    (USB ports facing down)

         3.3V [01] [02] 5V
   GPIOX_17  [03] [04] 5V           (I2C SDA)
   GPIOX_18  [05] [06] GND          (I2C SCL)
    GPIOX_5  [07] [08] GPIOX_12     (UART TX)
         GND [09] [10] GPIOX_13     (UART RX)
    GPIOX_3  [11] [12] GPIOAO_8
    GPIOX_4  [13] [14] GND
    GPIOX_7  [15] [16] GPIOX_0      (PWM_F)
        3.3V [17] [18] GPIOX_1
    GPIOX_8  [19] [20] GND
    GPIOX_9  [21] [22] GPIOX_2      ← Tested (GPIO 579)
   GPIOX_11  [23] [24] GPIOX_10
         GND [25] [26] GPIOX_16
  GPIOA_14   [27] [28] GPIOA_15     (I2C)
   GPIOX_14  [29] [30] GND
   GPIOX_15  [31] [32] GPIOX_19     (PWM_B)
    GPIOX_6  [33] [34] GND          (PWM_A)
   GPIOAO_7  [35] [36] GPIOH_5
   GPIOAO_9  [37] [38] GPIOAO_10
         GND [39] [40] GPIOAO_4
```

---

## GPIO to sysfs Number Mapping

### GPIOX Bank (Base: 512 + offset, but on Armbian = 577+)

| Pin | Function | sysfs GPIO | Notes               |
| --- | -------- | ---------- | ------------------- |
| 16  | GPIOX_0  | 577        |                     |
| 18  | GPIOX_1  | 578        |                     |
| 22  | GPIOX_2  | **579**    | ✅ Tested & working |
| 11  | GPIOX_3  | 580        |                     |
| 13  | GPIOX_4  | 581        |                     |
| 07  | GPIOX_5  | 582        |                     |
| 33  | GPIOX_6  | 583        | PWM_A               |
| 15  | GPIOX_7  | 584        | PWM_F               |
| 19  | GPIOX_8  | 585        |                     |
| 21  | GPIOX_9  | 586        |                     |
| 24  | GPIOX_10 | 587        |                     |
| 23  | GPIOX_11 | 588        |                     |
| 08  | GPIOX_12 | 589        | UART_A_TX           |
| 10  | GPIOX_13 | 590        | UART_A_RX           |
| 29  | GPIOX_14 | 591        | UART_A_CTS          |
| 31  | GPIOX_15 | 592        | UART_A_RTS          |
| 26  | GPIOX_16 | 593        | PWM_E               |
| 03  | GPIOX_17 | 594        | I2C_M2_SDA          |
| 05  | GPIOX_18 | 595        | I2C_M2_SCL          |
| 32  | GPIOX_19 | 596        | PWM_B               |

### GPIOAO Bank (Base: 597+)

| Pin | Function  | sysfs GPIO | Notes |
| --- | --------- | ---------- | ----- |
| 40  | GPIOAO_4  | 601        |       |
| 35  | GPIOAO_7  | 604        |       |
| 12  | GPIOAO_8  | 605        |       |
| 37  | GPIOAO_9  | 606        |       |
| 38  | GPIOAO_10 | 607        |       |

### Power Pins

| Pin                            | Function |
| ------------------------------ | -------- |
| 01, 17                         | 3.3V     |
| 02, 04                         | 5V       |
| 06, 09, 14, 20, 25, 30, 34, 39 | GND      |

---

## Basic GPIO Commands

### Export a GPIO Pin

```bash
sudo bash -c 'echo 579 > /sys/class/gpio/export'
```

### Set Pin Direction (Output)

```bash
sudo bash -c 'echo out > /sys/class/gpio/gpio579/direction'
```

### Set Pin HIGH

```bash
sudo bash -c 'echo 1 > /sys/class/gpio/gpio579/value'
```

### Set Pin LOW

```bash
sudo bash -c 'echo 0 > /sys/class/gpio/gpio579/value'
```

### Read Pin Value

```bash
cat /sys/class/gpio/gpio579/value
```

### Unexport Pin

```bash
sudo bash -c 'echo 579 > /sys/class/gpio/unexport'
```

---

## One-Liner Commands

### Set Single Pin HIGH

```bash
sudo bash -c 'echo 579 > /sys/class/gpio/export 2>/dev/null; echo out > /sys/class/gpio/gpio579/direction; echo 1 > /sys/class/gpio/gpio579/value'
```

### Set Multiple Pins HIGH

```bash
sudo bash -c 'for pin in 577 578 579 580 581 582; do echo $pin > /sys/class/gpio/export 2>/dev/null; echo out > /sys/class/gpio/gpio$pin/direction; echo 1 > /sys/class/gpio/gpio$pin/value; echo "GPIO $pin: HIGH"; done'
```

### Set All Pins LOW

```bash
sudo bash -c 'for pin in 577 578 579 580 581 582; do echo 0 > /sys/class/gpio/gpio$pin/value; echo "GPIO $pin: LOW"; done'
```

### Blink Test

```bash
sudo bash -c 'for i in 1 2 3; do echo 1 > /sys/class/gpio/gpio579/value; sleep 0.5; echo 0 > /sys/class/gpio/gpio579/value; sleep 0.5; done'
```

---

## Wiring Examples

### LED Connection

```
Pin 22 (GPIO 579) ──[330Ω resistor]──[LED +]──[LED -]── Pin 6 (GND)
```

### Relay Module Connection

```
Pin 22 (GPIO 579) ── Relay Signal (IN)
Pin 2  (5V)       ── Relay VCC
Pin 6  (GND)      ── Relay GND
```

### Button Connection (with pull-down)

```
Pin 3.3V ──[Button]── Pin 21 (GPIO 586)
                  └──[10kΩ resistor]── GND
```

---

## Wallbox Project Pin Configuration

Current configuration in `config/production.json`:

```json
{
  "gpio_pins": {
    "relay_enable": 579,
    "led_green": 577,
    "led_yellow": 578,
    "led_red": 580,
    "button": 586
  }
}
```

| Function     | sysfs GPIO | Physical Pin |
| ------------ | ---------- | ------------ |
| Relay Enable | 579        | Pin 22       |
| LED Green    | 577        | Pin 16       |
| LED Yellow   | 578        | Pin 18       |
| LED Red      | 580        | Pin 11       |
| Button       | 586        | Pin 21       |

---

## ⚠️ Safety Notes

1. **Voltage**: GPIO pins are **3.3V only** - never connect 5V directly!
2. **Current**: Max ~8mA per pin - use transistors/relays for higher loads
3. **Resistors**: Always use 220-330Ω resistors with LEDs
4. **ESD**: Handle with care to avoid static discharge damage
5. **Short Circuits**: Double-check connections before powering on

---

## Troubleshooting

### "Permission denied"

```bash
# Run as root
sudo -i
# Or use sudo bash -c '...'
```

### "Device or resource busy"

```bash
# Pin already exported, unexport first
sudo bash -c 'echo 579 > /sys/class/gpio/unexport'
```

### Pin not working

1. Check if pin is already in use by another function (I2C, SPI, UART)
2. Verify correct sysfs number
3. Check wiring and connections
4. Test with multimeter

---

## References

- [BananaPi M5 Wiki](https://wiki.banana-pi.org/Banana_Pi_BPI-M5)
- [Armbian Documentation](https://docs.armbian.com/)
- [Linux GPIO sysfs Interface](https://www.kernel.org/doc/Documentation/gpio/sysfs.txt)
