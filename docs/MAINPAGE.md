# Wallbox Control System v4.0 - API Documentation

@mainpage Wallbox Control System v4.0

@section intro_sec Introduction

Welcome to the Wallbox Control System v4.0 API documentation. This is the **latest production version** with complete feature set for ISO 15118 EV charging station control.

@section features_sec Key Features

@subsection http_api HTTP REST API

- Full remote control via HTTP on port 8080
- RESTful endpoints for status, control, and monitoring
- Compatible with web applications and mobile apps

@subsection cp_signal CP Signal System

- IEC 61851-1 compliant Control Pilot signal handling
- Hardware mode for BananaPi M5 GPIO integration
- Simulator mode for development and testing

@subsection iso15118 ISO 15118 Protocol

- Complete ISO 15118 stack implementation
- UDP communication on ports 50010/50011
- State machine with validated transitions

@subsection architecture Modern Architecture

- **ChargingStateMachine**: State pattern implementation
- **WallboxController**: Main application controller
- **HttpApiServer**: REST API server
- **UdpCommunicator**: ISO 15118 network layer
- **GPIO Controllers**: Platform abstraction (Stub/BananaPi)
- **CP Signal Readers**: Hardware/Simulator implementations

@section project_structure Project Structure

```
├── include/wallbox/       # Main header files
├── src/
│   ├── api/               # HTTP API server
│   ├── core/              # Core logic and controllers
│   ├── gpio/              # GPIO implementations
│   ├── network/           # UDP communication
│   ├── signal/            # CP signal handling
│   └── simulator/         # Wallbox simulator
├── config/                # Configuration files
├── scripts/               # Build, deploy, and test scripts
├── docs/                  # Documentation
└── WallboxCtrl/           # Legacy/alternative builds
```

@section versions_sec Version Information

@subsection v4_latest v4.0 - Latest (270KB) ⭐
**Recommended for production deployments**

Features:

- HTTP REST API (port 8080)
- UDP Communication (ISO 15118)
- CP Signal Support (Hardware + Simulator)
- ChargingStateMachine
- WallboxController
- Full modular architecture
- Dual mode: Interactive terminal or HTTP API

Build target: `wallbox_control_v4`

@subsection v3_prev v3.0 - Simplified (80KB)
**Minimal footprint version**

Features:

- Simple relay control
- GPIO pin 21 default
- Stub GPIO for testing
- BananaPi GPIO support

Build target: `wallbox_control_v3`

@subsection v2_older v2.0 - SOLID (141KB)
**Legacy SOLID architecture**

Features:

- SOLID principles
- UDP communication
- Legacy compatibility

Build target: `wallbox_control_v2`

@section quick_start Quick Start

@subsection build Building

```bash
cd <PROJECT_ROOT>
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

@subsection run Running

**Terminal 1 - Wallbox Controller v4.0:**

```bash
cd build/bin
./wallbox_control_v4 config/development.json
```

**Terminal 2 - Simulator:**

```bash
cd build/bin
./simulator
```

@subsection api_test HTTP API Testing

```bash
# Check status
curl http://localhost:8080/api/status

# Enable wallbox
curl -X POST http://localhost:8080/api/enable

# Start charging
curl -X POST http://localhost:8080/api/start
```

@section architecture_sec Architecture

The system follows modern C++ best practices with SOLID principles and design patterns:

- **Strategy Pattern**: GPIO abstraction (StubGpioController / BananaPiGpioController)
- **State Pattern**: ChargingStateMachine with validated transitions
- **Observer Pattern**: State change notifications
- **Factory Pattern**: Component creation and initialization
- **Dependency Injection**: Loose coupling between components
- **Command Pattern**: HTTP API request handling
- **Adapter Pattern**: UDP network communication

@section components Main Components

@subsection controller WallboxController
Main application controller orchestrating all components. Manages lifecycle, state transitions, and component communication.

Related files:

- WallboxController.h / WallboxController.cpp
- See @ref Wallbox::WallboxController

@subsection statemachine ChargingStateMachine
Implements charging state logic with enforced transitions:

- idle → ready → charging → stop

Related files:

- ChargingStateMachine.h / ChargingStateMachine.cpp
- See @ref Wallbox::ChargingStateMachine

@subsection httpserver HttpApiServer  
HTTP REST API server providing remote control interface on port 8080.

Related files:

- HttpApiServer.h / HttpApiServer.cpp
- See @ref Wallbox::HttpApiServer

@subsection udp UdpCommunicator
Handles ISO 15118 UDP messaging on ports 50010/50011.

Related files:

- UdpCommunicator.h / UdpCommunicator.cpp
- See @ref Wallbox::UdpCommunicator

@subsection gpio GPIO Controllers
Platform-independent GPIO control with multiple implementations.

Related files:

- IGpioController.h (Interface)
- StubGpioController.cpp (Development)
- BananaPiGpioController.cpp (Production)
- See @ref Wallbox::IGpioController

@section config Configuration

Configuration files are located in `config/` directory:

- `development.json` - Local development with simulator
- `development_pi.json` - BananaPi development
- `production.json` - Production deployment
- `test.json` - Testing configuration

Example configuration:

```json
{
  "mode": "development",
  "gpio": {
    "type": "stub",
    "relay_pin": 21
  },
  "network": {
    "udp_listen_port": 50010,
    "udp_send_port": 50011,
    "udp_send_address": "127.0.0.1",
    "api_port": 8080
  },
  "cp_signal": {
    "mode": "simulator"
  }
}
```

@section deployment Deployment

For BananaPi M5 production deployment:

```bash
# Build on target
cd ~/wallbox-src
mkdir -p build && cd build
cmake ..
make wallbox_control_v4 -j4

# Run
./bin/wallbox_control_v4 ../config/production.json
```

Or deploy from development machine:

```bash
./scripts/deploy/deploy.sh
```

@section hardware_sec Hardware Setup

For BananaPi M5 GPIO configuration, see @ref BANANAPI_M5_SETUP.md "BananaPi M5 Setup Guide"

GPIO Pins used:

- Pin 579 (GPIOH_4): Relay 1
- Pin 586 (GPIOH_5): Relay 2
- Pin 587 (GPIOH_6): Relay 3
- Pin 590 (GPIOH_7): Relay 4

@section docs_sec Documentation

- @ref QUICK_START.md "Quick Start Guide"
- @ref INSTALLATION_GUIDE.md "Installation Guide"
- @ref ARCHITECTURE_V3.md "Architecture Documentation"
- @ref API_REFERENCE.md "API Reference"
- @ref MODE_GUIDE.md "Operating Modes Guide"
- @ref BANANAPI_M5_SETUP.md "BananaPi M5 Setup"

@section license License

Copyright © 2025 Wallbox Control System Project

@author Wallbox Development Team
@version 4.0
@date December 2025
