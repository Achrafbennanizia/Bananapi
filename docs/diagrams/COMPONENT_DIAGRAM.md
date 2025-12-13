# Wallbox System - Component Diagram

**Date:** December 13, 2025  
**Version:** 4.1 (with CP Signal System)  
**Purpose:** High-level system components and interfaces

---

## UML Component Diagram

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    WALLBOX SYSTEM COMPONENT DIAGRAM                      │
└─────────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────────┐
│                          PRESENTATION LAYER                             │
└────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────┐       ┌─────────────────────────┐
│   HTTP REST API         │       │   React Web UI          │
│   «component»           │       │   «component»           │
├─────────────────────────┤       ├─────────────────────────┤
│                         │       │                         │
│ Ports:                  │       │ Dependencies:           │
│  ◄─ 8080/TCP (HTTP)     │       │  ─► HTTP API (8080)     │
│                         │       │  ─► Websocket (future)  │
│ Provided Interfaces:    │       │                         │
│  ● GET /status          │       │ UI Components:          │
│  ● POST /start          │       │  • Dashboard            │
│  ● POST /stop           │       │  • Status Display       │
│  ● POST /pause          │       │  • Control Buttons      │
│  ● POST /resume         │       │  • Configuration        │
│  ● GET /info            │       │  • Logs Viewer          │
│                         │       │                         │
│ Implementation:         │       │ Tech Stack:             │
│  • libmicrohttpd        │       │  • React 18.x           │
│  • JSON responses       │       │  • Axios HTTP client    │
│  • CORS enabled         │       │  • Material-UI          │
└─────────────────────────┘       └─────────────────────────┘
         │                                 │
         │ uses                            │ makes requests
         ▼                                 ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                            APPLICATION LAYER                             │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│              WALLBOX CONTROLLER «component»                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│ Required Interfaces:                                                     │
│  ○ IGpioController      ────────┐                                       │
│  ○ INetworkCommunicator ────────┼──────┐                                │
│  ○ ICpSignalReader      ────────┼──────┼───┐                            │
│                                  │      │   │                            │
│ Provided Services:               │      │   │                            │
│  ● startCharging()               │      │   │                            │
│  ● stopCharging()                │      │   │                            │
│  ● pauseCharging()               │      │   │                            │
│  ● resumeCharging()              │      │   │                            │
│  ● enableWallbox()               │      │   │                            │
│  ● disableWallbox()              │      │   │                            │
│  ● getStatusJson()               │      │   │                            │
│                                  │      │   │                            │
│ Internal Components:             │      │   │                            │
│  ┌────────────────────┐          │      │   │                            │
│  │ ChargingStateMachine│         │      │   │                            │
│  ├────────────────────┤          │      │   │                            │
│  │ • State transitions│          │      │   │                            │
│  │ • Validation logic │          │      │   │                            │
│  │ • Event callbacks  │          │      │   │                            │
│  └────────────────────┘          │      │   │                            │
│                                  │      │   │                            │
└──────────────────────────────────┼──────┼───┼────────────────────────────┘
                                   │      │   │
                                   │      │   │
                                   │      │   │
┌────────────────────────────────────────────────────────────────────────┐
│                         INFRASTRUCTURE LAYER                            │
└────────────────────────────────────────────────────────────────────────┘

┌──────────────────────┐    ┌──────────────────────┐    ┌──────────────────────┐
│  GPIO Component      │    │  Network Component   │    │  CP Signal Component │
│  «component»         │    │  «component»         │    │  «component»         │
├──────────────────────┤    ├──────────────────────┤    ├──────────────────────┤
│                      │◄───┤                      │◄───┤                      │
│ Interface:           │    │ Interface:           │    │ Interface:           │
│  ◎ IGpioController   │    │  ◎ INetworkComm     │    │  ◎ ICpSignalReader   │
│                      │    │                      │    │                      │
│ Implementations:     │    │ Implementation:      │    │ Implementations:     │
│  • BananaPiGpioCtrl  │    │  • UdpCommunicator   │    │  • HardwareCpReader  │
│  • StubGpioCtrl      │    │                      │    │  • SimulatorCpReader │
│                      │    │ Protocol:            │    │                      │
│ Hardware:            │    │  • UDP Datagrams     │    │ Factory:             │
│  ─► Linux sysfs      │    │  • Port 50010 (RX)   │    │  • CpSignalReaderFac │
│  ─► /sys/class/gpio  │    │  • Port 50011 (TX)   │    │                      │
│                      │    │                      │    │ Modes:               │
│ Pins Managed:        │    │ Message Format:      │    │  • Hardware (GPIO)   │
│  • Pin 2: Relay      │    │  [CMD][DATA...]      │    │  • Simulator (UDP)   │
│  • Pin 3: LED Green  │    │                      │    │                      │
│  • Pin 4: LED Yellow │    │ Features:            │    │ IEC 61851-1 States:  │
│  • Pin 5: LED Red    │    │  • Async receive     │    │  • STATE_A: 12V      │
│  • Pin 6: Button     │    │  • Thread-safe       │    │  • STATE_B: 9V       │
│  • Pin 7: CP Signal  │    │  • JSON serialization│    │  • STATE_C: 6V       │
└──────────────────────┘    └──────────────────────┘    │  • STATE_D: 3V       │
                                                         │  • STATE_E: 0V       │
                                                         │  • STATE_F: -12V     │
                                                         │                      │
                                                         │ Features:            │
                                                         │  • Voltage mapping   │
                                                         │  • Continuous monitor│
                                                         │  • State callbacks   │
                                                         │  • 100ms polling     │
                                                         └──────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                           EXTERNAL SYSTEMS                              │
└────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────┐       ┌─────────────────────────┐
│   Hardware Device       │       │   Development Simulator │
│   «external»            │       │   «external»            │
├─────────────────────────┤       ├─────────────────────────┤
│                         │       │                         │
│ Platform:               │       │ Purpose:                │
│  • Banana Pi M1         │       │  • Testing without HW   │
│  • ARM Cortex-A7        │       │  • CI/CD integration    │
│  • Debian-based OS      │       │  • Development mode     │
│                         │       │                         │
│ Physical Interfaces:    │       │ Simulation:             │
│  ◄─► GPIO pins (sysfs)  │       │  ─► UDP messages        │
│  ◄─► Ethernet           │       │  ─► Virtual states      │
│  ◄─► USB                │       │  ─► Mock responses      │
│                         │       │                         │
│ Connected Components:   │       │ Simulator Features:     │
│  • Power relay          │       │  • CP state injection   │
│  • Status LEDs          │       │  • Button press sim     │
│  • Control button       │       │  • Network control      │
│  • CP signal line (ADC) │       │  • State machine test   │
│  • Electric vehicle     │       │                         │
└─────────────────────────┘       └─────────────────────────┘
         △                                 △
         │ GPIO & CP signal                │ UDP
         │                                 │
    ┌────┴────┐                       ┌────┴────┐
    │         │                       │         │
    ▼         ▼                       ▼         ▼


┌────────────────────────────────────────────────────────────────────────┐
│                        CONFIGURATION COMPONENT                          │
└────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│              CONFIGURATION «component» «singleton»                       │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│ Configuration Sources:                                                   │
│  1. Environment variables (highest priority)                            │
│  2. JSON file: /etc/wallbox/config.json                                │
│  3. Default values (fallback)                                           │
│                                                                          │
│ Configuration Parameters:                                               │
│  • mode: "simulator" | "hardware"                                       │
│  • udp_listen_port: 50010                                               │
│  • udp_send_port: 50011                                                 │
│  • api_port: 8080                                                       │
│  • relay_pin: 2                                                         │
│  • led_green_pin: 3                                                     │
│  • led_yellow_pin: 4                                                    │
│  • led_red_pin: 5                                                       │
│  • button_pin: 6                                                        │
│  • cp_pin: 7                                                            │
│  • max_current_amps: 32                                                 │
│  • voltage: 230                                                         │
│                                                                          │
│ JSON Schema:                                                            │
│  {                                                                       │
│    "mode": "hardware",                                                   │
│    "network": {                                                          │
│      "udp_listen_port": 50010,                                          │
│      "udp_send_port": 50011,                                            │
│      "api_port": 8080                                                   │
│    },                                                                    │
│    "gpio": {                                                             │
│      "relay_pin": 2,                                                    │
│      "led_green_pin": 3,                                                │
│      "cp_pin": 7                                                        │
│    },                                                                    │
│    "charging": {                                                         │
│      "max_current_amps": 32,                                            │
│      "voltage": 230                                                     │
│    }                                                                     │
│  }                                                                       │
└─────────────────────────────────────────────────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                         COMPONENT DEPENDENCIES                          │
└────────────────────────────────────────────────────────────────────────┘

                    ┌─────────────────┐
                    │   React Web UI  │
                    └────────┬────────┘
                             │ HTTP
                             ▼
                    ┌─────────────────┐
                    │  HTTP REST API  │
                    └────────┬────────┘
                             │ uses
                             ▼
        ┌────────────────────────────────────────┐
        │      Wallbox Controller Component      │
        │                                        │
        │  ┌──────────────────────────────┐     │
        │  │  Charging State Machine      │     │
        │  └──────────────────────────────┘     │
        └───┬─────────────┬─────────────┬────────┘
            │             │             │
            │ requires    │ requires    │ requires
            ▼             ▼             ▼
  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
  │ GPIO Comp    │ │ Network Comp │ │ CP Signal    │
  └──────┬───────┘ └──────┬───────┘ └──────┬───────┘
         │                │                │
         │                │                │ delegates to
         │                │                ├─────────────┐
         │                │                │             │
         ▼                ▼                ▼             ▼
  ┌──────────────┐ ┌──────────────┐ ┌──────────┐ ┌──────────┐
  │ HW Platform  │ │ UDP Network  │ │ HW Mode  │ │ Sim Mode │
  │ (Banana Pi)  │ │ (127.0.0.1)  │ │ (GPIO)   │ │ (UDP)    │
  └──────────────┘ └──────────────┘ └──────────┘ └──────────┘

                    ▲
                    │ reads from
                    │
              ┌─────────────┐
              │ Config File │
              │  (JSON)     │
              └─────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                        DEPLOYMENT SCENARIOS                             │
└────────────────────────────────────────────────────────────────────────┘

[SCENARIO 1: Production Deployment on Banana Pi]

┌─────────────────────────────────────────────────────────────────────────┐
│  Banana Pi M1 Hardware                                                   │
│  ┌────────────────────────────────────────────────────────────────┐     │
│  │  Debian Linux OS                                               │     │
│  │  ┌──────────────────────────────────────────────────────────┐  │     │
│  │  │  Docker Container: wallbox-system                         │  │     │
│  │  │  ┌────────────────────────────────────────────────────┐   │  │     │
│  │  │  │  Wallbox Controller (wallbox_control_v4)           │   │  │     │
│  │  │  │  Mode: "hardware"                                  │   │  │     │
│  │  │  │  ├─ GPIO Component (BananaPiGpioController)       │   │  │     │
│  │  │  │  ├─ Network Component (UdpCommunicator)           │   │  │     │
│  │  │  │  └─ CP Signal (HardwareCpSignalReader)            │   │  │     │
│  │  │  └────────────────────────────────────────────────────┘   │  │     │
│  │  │  ┌────────────────────────────────────────────────────┐   │  │     │
│  │  │  │  HTTP API Server (port 8080)                       │   │  │     │
│  │  │  └────────────────────────────────────────────────────┘   │  │     │
│  │  └──────────────────────────────────────────────────────────┘  │     │
│  │                          │                                      │     │
│  │                          │ GPIO access                          │     │
│  │                          ▼                                      │     │
│  │  ┌──────────────────────────────────────────────────────────┐  │     │
│  │  │  /sys/class/gpio/ (sysfs)                                │  │     │
│  │  └──────────────────────────────────────────────────────────┘  │     │
│  └────────────────────────────────────────────────────────────────┘     │
│                          │                                               │
│                          │ Physical pins                                 │
│                          ▼                                               │
│  ┌──────────────────────────────────────────────────────────────┐       │
│  │  Hardware I/O                                                 │       │
│  │  • Relay control (Pin 2)                                     │       │
│  │  • Status LEDs (Pins 3,4,5)                                  │       │
│  │  • Control button (Pin 6)                                    │       │
│  │  • CP signal ADC (Pin 7)                                     │       │
│  └──────────────────────────────────────────────────────────────┘       │
└─────────────────────────────────────────────────────────────────────────┘
                          │
                          │ CP signal from vehicle
                          ▼
                    ┌──────────────┐
                    │ Electric     │
                    │ Vehicle (EV) │
                    └──────────────┘


[SCENARIO 2: Development Environment (Simulator)]

┌─────────────────────────────────────────────────────────────────────────┐
│  Development Machine (Laptop/Desktop)                                    │
│  ┌────────────────────────────────────────────────────────────────┐     │
│  │  ┌──────────────────────────────────────────────────────────┐  │     │
│  │  │  Wallbox Controller (simulator mode)                     │  │     │
│  │  │  Mode: "simulator"                                       │  │     │
│  │  │  ├─ GPIO Component (StubGpioController)                 │  │     │
│  │  │  ├─ Network Component (UdpCommunicator)                 │  │     │
│  │  │  └─ CP Signal (SimulatorCpSignalReader)                 │  │     │
│  │  └──────────────────────────────────────────────────────────┘  │     │
│  │                          │                                      │     │
│  │                          │ UDP 127.0.0.1:50010                  │     │
│  │                          ▼                                      │     │
│  │  ┌──────────────────────────────────────────────────────────┐  │     │
│  │  │  Simulator Program (test_interactive)                    │  │     │
│  │  │  • Sends CP state messages via UDP                       │  │     │
│  │  │  • Simulates button presses                              │  │     │
│  │  │  • Command-line interface                                │  │     │
│  │  └──────────────────────────────────────────────────────────┘  │     │
│  │                                                                 │     │
│  │  ┌──────────────────────────────────────────────────────────┐  │     │
│  │  │  React Web UI (dev server: port 3000)                    │  │     │
│  │  │  • Hot reload                                             │  │     │
│  │  │  • API proxy to :8080                                    │  │     │
│  │  └──────────────────────────────────────────────────────────┘  │     │
│  └────────────────────────────────────────────────────────────────┘     │
└─────────────────────────────────────────────────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                        COMPONENT INTERFACES                             │
└────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│  «interface» IGpioController                                             │
├─────────────────────────────────────────────────────────────────────────┤
│  + initialize(): bool                                                    │
│  + shutdown(): void                                                      │
│  + setPinMode(pin: int, mode: PinMode): bool                            │
│  + digitalWrite(pin: int, value: PinValue): bool                        │
│  + digitalRead(pin: int): PinValue                                      │
│  + isInitialized(): bool                                                │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│  «interface» INetworkCommunicator                                        │
├─────────────────────────────────────────────────────────────────────────┤
│  + connect(): bool                                                       │
│  + disconnect(): void                                                    │
│  + send(data: vector<uint8_t>): bool                                    │
│  + startReceiving(callback: function): void                             │
│  + stopReceiving(): void                                                │
│  + isConnected(): bool                                                  │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│  «interface» ICpSignalReader                                             │
├─────────────────────────────────────────────────────────────────────────┤
│  + initialize(): bool                                                    │
│  + shutdown(): void                                                      │
│  + readCpState(): CpState                                               │
│  + getCpStateString(): string                                           │
│  + onStateChange(callback: function<void(CpState, CpState)>): void      │
│  + startMonitoring(): bool                                              │
│  + stopMonitoring(): void                                               │
└─────────────────────────────────────────────────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                          PORT MAPPING                                   │
└────────────────────────────────────────────────────────────────────────┘

| Port  | Protocol | Service              | Direction | Purpose               |
|-------|----------|----------------------|-----------|-----------------------|
| 8080  | HTTP     | REST API             | Inbound   | External control      |
| 50010 | UDP      | Command receive      | Inbound   | Simulator messages    |
| 50011 | UDP      | Status transmit      | Outbound  | Status updates        |
| 3000  | HTTP     | React dev server     | Inbound   | Web UI (dev only)     |


┌────────────────────────────────────────────────────────────────────────┐
│                        COMPONENT METRICS                                │
└────────────────────────────────────────────────────────────────────────┘

| Component | LOC | Files | Complexity | External Deps |
|-----------|-----|-------|------------|---------------|
| Wallbox Controller | 673 | 2 | High | 4 interfaces |
| GPIO Component | 360 | 4 | Medium | Linux sysfs |
| Network Component | 198 | 2 | Medium | POSIX sockets |
| CP Signal Component | 857 | 7 | Medium | GPIO/Network |
| State Machine | 283 | 2 | Medium | None |
| HTTP API | 347 | 2 | Medium | libmicrohttpd |
| Configuration | 298 | 2 | Low | JSON parser |
| React UI | ~1200 | 15 | Medium | React, Axios |

**Total System**: ~4,216 lines (C++), ~1,200 lines (React)

---

## System Boundaries

```

┌──────────────────────────────────────────────────────────────────────┐
│ SYSTEM BOUNDARY │
│ │
│ ┌────────────────────────────────────────────────────────────────┐ │
│ │ APPLICATION COMPONENTS │ │
│ │ (Owned by project, compiled into binaries) │ │
│ └────────────────────────────────────────────────────────────────┘ │
│ │
│ ┌────────────────────────────────────────────────────────────────┐ │
│ │ EXTERNAL DEPENDENCIES │ │
│ │ • libmicrohttpd (HTTP server) │ │
│ │ • POSIX threads (std::thread) │ │
│ │ • POSIX sockets (UDP) │ │
│ │ • Linux sysfs (GPIO) │ │
│ │ • JSON parser (internal) │ │
│ └────────────────────────────────────────────────────────────────┘ │
│ │
│ ┌────────────────────────────────────────────────────────────────┐ │
│ │ HARDWARE INTERFACES │ │
│ │ • Banana Pi GPIO pins (7 pins) │ │
│ │ • Network interface (Ethernet/WiFi) │ │
│ └────────────────────────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────────────────────────┘

```

---

**Generated:** December 13, 2025
**Version:** 4.1 with CP Signal System
```
