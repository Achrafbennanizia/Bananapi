# Wallbox Controller - Class Architecture & Interactions

**Version:** 3.0 (Dual Mode with C++14 Support)  
**Date:** December 13, 2025  
**Purpose:** Complete guide to system architecture, class responsibilities, and interactions

---

## 📋 Table of Contents

1. [System Overview](#system-overview)
2. [Core Classes](#core-classes)
3. [Interface Abstractions](#interface-abstractions)
4. [Concrete Implementations](#concrete-implementations)
5. [Support Classes](#support-classes)
6. [Class Interaction Diagrams](#class-interaction-diagrams)
7. [Design Patterns Used](#design-patterns-used)
8. [Data Flow](#data-flow)
9. [State Management](#state-management)

---

## 🎯 System Overview

The Wallbox Controller is a modular, SOLID-principle-based C++ application for managing electric vehicle charging stations. It supports dual-mode operation (API + Interactive) and communicates via UDP with ISO 15118 protocol.

### Architecture Layers

```
┌─────────────────────────────────────────────────────────┐
│                   Application Layer                     │
│  (main_v3.cpp, main_v4.cpp - Entry Points)            │
└─────────────────────────────────────────────────────────┘
                           │
┌─────────────────────────────────────────────────────────┐
│                   Controller Layer                      │
│  WallboxController, SimpleWallboxController            │
│  HttpApiServer, Application                            │
└─────────────────────────────────────────────────────────┘
                           │
┌─────────────────────────────────────────────────────────┐
│                  Business Logic Layer                   │
│  ChargingStateMachine, Configuration                   │
└─────────────────────────────────────────────────────────┘
                           │
┌─────────────────────────────────────────────────────────┐
│               Hardware Abstraction Layer                │
│  IGpioController → BananaPiGpio / StubGpio            │
│  INetworkCommunicator → UdpCommunicator               │
└─────────────────────────────────────────────────────────┘
                           │
┌─────────────────────────────────────────────────────────┐
│                    System Layer                         │
│  Linux GPIO (sysfs), UDP Sockets, ISO 15118           │
└─────────────────────────────────────────────────────────┘
```

---

## 📦 Core Classes

### 1. WallboxController

**Location:** `WallboxCtrl/include/WallboxController.h`  
**Responsibility:** Main orchestrator for the wallbox system

```cpp
class WallboxController {
    // Manages overall wallbox operation
    // Coordinates GPIO, Network, and State Machine

private:
    std::unique_ptr<IGpioController> m_gpio;
    std::unique_ptr<INetworkCommunicator> m_network;
    std::unique_ptr<ChargingStateMachine> m_stateMachine;
    std::atomic<bool> m_running;
    bool m_relayEnabled;
    bool m_wallboxEnabled;
};
```

#### Responsibilities:

- 🔌 **Hardware Control**: Manages GPIO pins (relay, LEDs)
- 🌐 **Network Communication**: Sends/receives UDP messages
- 🔄 **State Coordination**: Integrates with ChargingStateMachine
- 📊 **Status Reporting**: Provides system status to API/Interactive modes
- 🎯 **Command Execution**: Processes start/stop/pause/resume commands

#### Key Methods:

```cpp
bool initialize()              // Setup GPIO and network
void run()                     // Main event loop
bool startCharging()           // Begin charging session
bool stopCharging()            // End charging session
bool pauseCharging()           // Pause current session
bool resumeCharging()          // Resume paused session
void enableWallbox()           // Enable wallbox operation
void disableWallbox()          // Disable for maintenance
ChargingState getCurrentState() // Get current state
void handleNetworkMessage()    // Process ISO 15118 messages
```

#### Design Patterns:

- **Facade Pattern**: Simplifies complex subsystem interactions
- **Observer Pattern**: Listens to state machine changes
- **Dependency Injection**: Receives interfaces via constructor

---

### 2. SimpleWallboxController

**Location:** `WallboxCtrl/include/SimpleWallboxController.h`  
**Responsibility:** Simplified version for v4.0 - relay-only control

```cpp
class SimpleWallboxController {
    // Minimal implementation focusing on relay control

private:
    std::unique_ptr<IGpioController> m_gpio;
    int m_relayPin;
    bool m_relayEnabled;
};
```

#### Responsibilities:

- ⚡ **Relay Control**: Direct ON/OFF control of charging relay
- 🎛️ **Simple Interface**: Minimal API for basic operations
- 🔧 **Maintenance Mode**: Easy debugging and testing

#### Key Methods:

```cpp
bool enableRelay()   // Turn relay ON
bool disableRelay()  // Turn relay OFF
bool isRelayEnabled() // Check relay status
```

---

### 3. ChargingStateMachine

**Location:** `WallboxCtrl/include/ChargingStateMachine.h`  
**Responsibility:** Manages charging lifecycle states

```cpp
enum class ChargingState {
    IDLE,       // Ready, no vehicle
    PREPARING,  // Vehicle detected, preparing
    CHARGING,   // Active charging
    PAUSED,     // Temporarily paused
    FINISHING,  // Session completing
    ERROR,      // Error condition
    DISABLED    // Maintenance mode
};

class ChargingStateMachine {
private:
    ChargingState m_currentState;
    std::vector<StateChangeCallback> m_listeners;
};
```

#### Responsibilities:

- 🔄 **State Transitions**: Validates and executes state changes
- 📢 **Event Notification**: Notifies observers of state changes
- ✅ **Validation**: Ensures only valid transitions occur
- 📊 **State Queries**: Provides current state information

#### State Transition Rules:

```
IDLE → PREPARING → CHARGING ⇄ PAUSED
                     ↓
                 FINISHING → IDLE

Any State → ERROR → IDLE (after reset)
Any State → DISABLED
```

#### Key Methods:

```cpp
bool transitionTo(ChargingState newState)  // Execute transition
bool canTransitionTo(ChargingState) const  // Check if valid
bool startCharging()                       // IDLE → PREPARING → CHARGING
bool stopCharging()                        // CHARGING → FINISHING → IDLE
bool pauseCharging()                       // CHARGING → PAUSED
bool resumeCharging()                      // PAUSED → CHARGING
void addStateChangeListener(callback)      // Register observer
```

#### Design Patterns:

- **State Pattern**: Encapsulates state-specific behavior
- **Observer Pattern**: Notifies listeners of changes
- **Command Pattern**: State transitions as commands

---

### 4. HttpApiServer

**Location:** `WallboxCtrl/include/HttpApiServer.h`  
**Responsibility:** REST API server for web/React integration

```cpp
class HttpApiServer {
private:
    int m_port;
    std::atomic<bool> m_running;
    std::map<std::string, HttpHandler> m_routes;
    struct MHD_Daemon* m_daemon;
};
```

#### Responsibilities:

- 🌐 **HTTP Server**: Serves REST API endpoints
- 🔌 **CORS Support**: Enables React app communication
- 📡 **Request Routing**: Maps URLs to handlers
- 📤 **JSON Responses**: Returns structured data

#### API Endpoints:

```
GET  /api/status     - Current wallbox status
GET  /api/info       - System information
GET  /api/config     - Configuration details
POST /api/start      - Start charging session
POST /api/stop       - Stop charging session
POST /api/pause      - Pause charging
POST /api/resume     - Resume charging
POST /api/enable     - Enable wallbox
POST /api/disable    - Disable wallbox
```

#### Key Methods:

```cpp
bool start()                                   // Start HTTP server
void stop()                                    // Stop server
void registerRoute(method, path, handler)      // Add endpoint
void GET/POST/PUT/DELETE(path, handler)       // Convenience methods
```

#### Design Patterns:

- **Command Pattern**: Each endpoint is a command
- **Factory Pattern**: Creates appropriate responses
- **Facade Pattern**: Simplifies access to wallbox

---

### 5. Application

**Location:** `WallboxCtrl/include/Application.h`  
**Responsibility:** Main application coordinator for dual mode

```cpp
class Application {
private:
    std::unique_ptr<WallboxController> m_controller;
    std::unique_ptr<HttpApiServer> m_apiServer;
    std::string m_mode; // "api", "interactive", "dual"
};
```

#### Responsibilities:

- 🚀 **Application Lifecycle**: Startup, run, shutdown
- 🎭 **Mode Management**: Switches between API/Interactive/Dual
- 🔗 **Component Wiring**: Connects controller to API server
- ⚙️ **Configuration**: Loads and applies settings

#### Key Methods:

```cpp
bool initialize()              // Setup all components
void run()                     // Main application loop
void setMode(string mode)      // Switch operational mode
void handleInteractiveMode()   // Command-line interface
void handleApiMode()           // REST API server mode
void handleDualMode()          // Both simultaneously
```

---

## 🔌 Interface Abstractions

### IGpioController

**Location:** `WallboxCtrl/include/IGpioController.h`  
**Purpose:** Hardware abstraction for GPIO operations

```cpp
class IGpioController {
public:
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool setPinMode(int pin, PinMode mode) = 0;
    virtual bool digitalWrite(int pin, PinValue value) = 0;
    virtual PinValue digitalRead(int pin) const = 0;
    virtual bool isInitialized() const = 0;
};
```

#### Why Interface?

- ✅ **Testability**: Allows mock implementations
- ✅ **Flexibility**: Swap hardware without code changes
- ✅ **Platform Independence**: Works on any Pi variant
- ✅ **SOLID**: Dependency Inversion Principle

#### Implementations:

1. **BananaPiGpioController** - Real hardware via sysfs
2. **StubGpioController** - Simulated GPIO for development

---

### INetworkCommunicator

**Location:** `WallboxCtrl/include/INetworkCommunicator.h`  
**Purpose:** Network communication abstraction

```cpp
class INetworkCommunicator {
public:
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool send(const std::vector<uint8_t>& data) = 0;
    virtual void startReceiving(MessageCallback callback) = 0;
    virtual void stopReceiving() = 0;
    virtual bool isConnected() const = 0;
};
```

#### Why Interface?

- ✅ **Protocol Agnostic**: UDP, TCP, mock, etc.
- ✅ **Testing**: Easy to mock network
- ✅ **Flexibility**: Change protocol without refactoring

#### Implementations:

1. **UdpCommunicator** - UDP socket communication

---

## 🛠️ Concrete Implementations

### BananaPiGpioController

**Location:** `WallboxCtrl/src/BananaPiGpioController.cpp`  
**Purpose:** Real GPIO control via Linux sysfs

```cpp
class BananaPiGpioController : public IGpioController {
private:
    bool m_initialized;
    static constexpr const char* GPIO_PATH = "/sys/class/gpio";

    bool exportPin(int pin);
    bool setDirection(int pin, string direction);
    bool setValue(int pin, int value);
};
```

#### How It Works:

1. **Export Pin**: Write pin number to `/sys/class/gpio/export`
2. **Set Direction**: Write "out" or "in" to `gpio{N}/direction`
3. **Write Value**: Write "0" or "1" to `gpio{N}/value`
4. **Read Value**: Read from `gpio{N}/value`

#### Pin Configuration (from config.json):

```json
{
  "gpio_pins": {
    "relay_enable": 21, // Main relay
    "led_green": 17, // Status LED
    "led_yellow": 27, // Warning LED
    "led_red": 22, // Error LED
    "button": 23 // User input
  }
}
```

---

### StubGpioController

**Location:** `WallboxCtrl/src/StubGpioController.cpp`  
**Purpose:** Simulated GPIO for development/testing

```cpp
class StubGpioController : public IGpioController {
private:
    std::map<int, PinMode> m_pinModes;
    std::map<int, PinValue> m_pinValues;
};
```

#### Use Cases:

- 💻 **Development**: Test without hardware
- 🧪 **Unit Testing**: Predictable behavior
- 🔍 **Debugging**: Inspect pin states
- 📚 **Demo Mode**: Show functionality without Pi

---

### UdpCommunicator

**Location:** `WallboxCtrl/src/UdpCommunicator.cpp`  
**Purpose:** UDP network communication for ISO 15118

```cpp
class UdpCommunicator : public INetworkCommunicator {
private:
    int m_listenPort;      // 50010 - wallbox listens
    int m_sendPort;        // 50011 - simulator listens
    std::string m_sendAddress;  // Target IP
    int m_socketFd;
    std::thread m_receiveThread;
};
```

#### Communication Flow:

```
Wallbox (50010)  ←──UDP──→  Simulator (50011)
    ↓                            ↓
Send state updates       Receive commands
Receive commands         Send state updates
```

#### Message Format (ISO 15118):

- Binary protocol
- State information (IDLE, CHARGING, etc.)
- Command messages (START, STOP, PAUSE)
- Synchronization packets

---

## 📊 Class Interaction Diagrams

### 1. System Initialization

```
┌─────────────┐
│   main()    │
└──────┬──────┘
       │
       ├─→ Create GpioFactory
       │   └─→ Returns IGpioController
       │       (BananaPi or Stub)
       │
       ├─→ Create UdpCommunicator
       │   (implements INetworkCommunicator)
       │
       ├─→ Create WallboxController
       │   │
       │   ├─→ Inject IGpioController
       │   ├─→ Inject INetworkCommunicator
       │   └─→ Create ChargingStateMachine
       │
       ├─→ Create HttpApiServer
       │   └─→ Register routes
       │
       ├─→ Create Application
       │   ├─→ Link WallboxController
       │   ├─→ Link HttpApiServer
       │   └─→ Set mode (api/interactive/dual)
       │
       └─→ Application.run()
```

### 2. Start Charging Sequence

```
User/API
   │
   ├─→ POST /api/start
   │   or
   └─→ Interactive command: "start"
       │
       ▼
┌─────────────────┐
│ HttpApiServer   │
│ or Application  │
└────────┬────────┘
         │
         ▼
┌──────────────────────┐
│ WallboxController    │
│  startCharging()     │
└──────────┬───────────┘
           │
           ├─→ Check m_wallboxEnabled
           │
           ├─→ ChargingStateMachine
           │   └─→ transitionTo(PREPARING)
           │       └─→ Validate transition
           │       └─→ Notify listeners
           │       └─→ transitionTo(CHARGING)
           │
           ├─→ IGpioController
           │   └─→ digitalWrite(relay, HIGH)
           │
           ├─→ INetworkCommunicator
           │   └─→ send(START_CHARGING message)
           │
           └─→ Update LEDs
               └─→ Green LED ON
```

### 3. State Change Propagation (Observer Pattern)

```
┌─────────────────────────┐
│ ChargingStateMachine    │
│  transitionTo(CHARGING) │
└────────┬────────────────┘
         │
         ├─→ Validate transition
         ├─→ Update m_currentState
         │
         └─→ notifyStateChange()
             │
             ├─→ Listener 1: WallboxController
             │   └─→ onStateChange()
             │       ├─→ Update GPIO (LEDs)
             │       └─→ Send UDP notification
             │
             ├─→ Listener 2: HttpApiServer
             │   └─→ Update cached status
             │
             └─→ Listener 3: Application
                 └─→ Log state change
```

### 4. UDP Message Handling

```
Simulator                 Wallbox
    │                        │
    │  START_CHARGING msg    │
    ├───────────────────────→│
    │                        │
    │                 ┌──────▼──────┐
    │                 │ UdpComm     │
    │                 │ receiveLoop │
    │                 └──────┬──────┘
    │                        │
    │                 ┌──────▼─────────────┐
    │                 │ MessageCallback    │
    │                 │ (WallboxController)│
    │                 └──────┬─────────────┘
    │                        │
    │                 ┌──────▼──────────────┐
    │                 │ handleNetworkMessage│
    │                 │  - Parse ISO 15118  │
    │                 │  - Execute command  │
    │                 └──────┬──────────────┘
    │                        │
    │   STATUS_UPDATE msg    │
    │←───────────────────────┤
    │                        │
```

### 5. Dual Mode Architecture

```
┌────────────────────────────────────────┐
│          Application (Dual Mode)       │
└────────┬───────────────────────┬───────┘
         │                       │
    ┌────▼─────┐           ┌────▼────────┐
    │ Thread 1 │           │  Thread 2   │
    │ HTTP API │           │ Interactive │
    └────┬─────┘           └────┬────────┘
         │                      │
         └──────────┬───────────┘
                    │
         ┌──────────▼──────────────┐
         │  WallboxController      │
         │  (Shared, thread-safe)  │
         └──────────┬──────────────┘
                    │
         ┌──────────┼──────────────┐
         │          │              │
    ┌────▼────┐ ┌──▼──┐ ┌────────▼─────┐
    │  GPIO   │ │State│ │   Network    │
    │Controller│ │Mach.│ │ Communicator │
    └─────────┘ └─────┘ └──────────────┘
```

---

## 🎨 Design Patterns Used

### 1. Strategy Pattern

**Used in:** IGpioController, INetworkCommunicator

```cpp
// Strategy: Different GPIO implementations
class WallboxController {
    std::unique_ptr<IGpioController> m_gpio; // Strategy

    // Can use BananaPiGpio or StubGpio interchangeably
};
```

**Benefits:**

- Swap implementations at runtime
- Easy testing with mock objects
- Platform independence

---

### 2. Observer Pattern

**Used in:** ChargingStateMachine

```cpp
// Observable
class ChargingStateMachine {
    std::vector<StateChangeCallback> m_listeners;

    void addStateChangeListener(StateChangeCallback cb) {
        m_listeners.push_back(cb);
    }

    void notifyStateChange() {
        for (auto& listener : m_listeners) {
            listener(oldState, newState, reason);
        }
    }
};

// Observer
wallbox->getStateMachine()->addStateChangeListener(
    [this](auto old, auto new, auto reason) {
        onStateChange(old, new, reason);
    }
);
```

**Benefits:**

- Loose coupling between components
- Multiple observers per event
- Easy to add new observers

---

### 3. State Pattern

**Used in:** ChargingStateMachine

```cpp
enum class ChargingState {
    IDLE, PREPARING, CHARGING, PAUSED, FINISHING, ERROR, DISABLED
};

// State-specific behaviors
bool transitionTo(ChargingState newState) {
    if (!isValidTransition(m_currentState, newState)) {
        return false; // Invalid transition
    }
    // Execute transition
}
```

**Benefits:**

- Clear state definitions
- Validated transitions
- State-specific behavior

---

### 4. Facade Pattern

**Used in:** WallboxController, HttpApiServer

```cpp
// Facade: Simplifies complex subsystem
class WallboxController {
    bool startCharging() {
        // Coordinates multiple subsystems:
        // - State machine
        // - GPIO control
        // - Network communication
        // - LED updates
    }
};
```

**Benefits:**

- Simple interface to complex system
- Hides implementation details
- Reduces coupling

---

### 5. Dependency Injection

**Used in:** All major classes

```cpp
// Constructor injection
WallboxController(
    std::unique_ptr<IGpioController> gpio,
    std::unique_ptr<INetworkCommunicator> network
) : m_gpio(std::move(gpio)), m_network(std::move(network)) {}
```

**Benefits:**

- Testability
- Flexibility
- Loose coupling
- SOLID principles

---

### 6. Factory Pattern

**Used in:** GpioFactory

```cpp
class GpioFactory {
    static std::unique_ptr<IGpioController> create(bool useRealHardware) {
        if (useRealHardware) {
            return std::make_unique<BananaPiGpioController>();
        } else {
            return std::make_unique<StubGpioController>();
        }
    }
};
```

**Benefits:**

- Centralized object creation
- Easy to add new types
- Hides construction complexity

---

## 🔄 Data Flow

### 1. Command Flow (User → Hardware)

```
┌──────┐     ┌────────────┐     ┌───────────────┐
│ User │────→│HTTP API or │────→│ Wallbox       │
└──────┘     │Interactive │     │ Controller    │
             └────────────┘     └───────┬───────┘
                                        │
                 ┌──────────────────────┼────────────────┐
                 │                      │                │
          ┌──────▼──────┐     ┌────────▼───────┐  ┌────▼─────┐
          │ChargingState│     │IGpioController │  │INetwork  │
          │  Machine    │     │(Relay control) │  │Comm.     │
          └─────────────┘     └────────────────┘  └──────────┘
                                      │                 │
                              ┌───────▼────────┐  ┌─────▼──────┐
                              │Physical Relay  │  │UDP Socket  │
                              │(GPIO Pin 21)   │  │(Port 50010)│
                              └────────────────┘  └────────────┘
```

### 2. Status Flow (Hardware → User)

```
┌────────────┐     ┌───────────────┐     ┌──────────────┐
│UDP Message │────→│ Wallbox       │────→│ State        │
│from        │     │ Controller    │     │ Machine      │
│Simulator   │     └───────┬───────┘     └──────┬───────┘
└────────────┘             │                    │
                           │                    │
                     ┌─────▼────────────────────▼─────┐
                     │    Observer Notifications      │
                     └────────────┬───────────────────┘
                                  │
                 ┌────────────────┼────────────────┐
                 │                │                │
        ┌────────▼────────┐  ┌────▼──────┐  ┌─────▼─────┐
        │HTTP API (JSON)  │  │Interactive│  │UDP Reply  │
        │/api/status      │  │Display    │  │to         │
        └─────────────────┘  └───────────┘  │Simulator  │
                │                            └───────────┘
        ┌───────▼────────┐
        │ React Web App  │
        └────────────────┘
```

---

## 🔐 State Management

### Charging State Machine Detailed States

```
┌──────────────────────────────────────────────────────┐
│                    IDLE STATE                        │
│  • Wallbox ready                                     │
│  • No vehicle connected                              │
│  • Relay OFF                                         │
│  • Waiting for vehicle                               │
│  • Valid transitions: → PREPARING, → DISABLED        │
└────────────────┬─────────────────────────────────────┘
                 │ Vehicle detected
                 ▼
┌──────────────────────────────────────────────────────┐
│                 PREPARING STATE                      │
│  • Vehicle detected                                  │
│  • Performing safety checks                          │
│  • Establishing communication                        │
│  • Relay still OFF                                   │
│  • Valid transitions: → CHARGING, → ERROR            │
└────────────────┬─────────────────────────────────────┘
                 │ Safety checks passed
                 ▼
┌──────────────────────────────────────────────────────┐
│                 CHARGING STATE                       │
│  • Active power transfer                             │
│  • Relay ON                                          │
│  • Monitoring current/voltage                        │
│  • ISO 15118 communication active                    │
│  • Valid transitions: → PAUSED, → FINISHING, → ERROR│
└──────┬──────────────────────────────────────┬────────┘
       │ User pause                            │ Complete
       ▼                                       ▼
┌──────────────────┐                  ┌──────────────────┐
│  PAUSED STATE    │                  │ FINISHING STATE  │
│  • Relay OFF     │                  │  • Stopping      │
│  • Session       │                  │  • Final checks  │
│    maintained    │                  │  • Relay OFF     │
│  • Can resume    │                  └────────┬─────────┘
└──────┬───────────┘                           │
       │ Resume                                 │
       └────────────────┬──────────────────────┘
                        ▼
┌──────────────────────────────────────────────────────┐
│                   IDLE STATE                         │
│  • Ready for next session                            │
└──────────────────────────────────────────────────────┘

                    ┌──────────────────┐
                    │   ERROR STATE    │
                    │  • Fault         │
Any State ─────────→│  • Relay OFF     │
                    │  • Needs reset   │
                    └────────┬─────────┘
                             │ Reset
                             ▼
                    ┌──────────────────┐
                    │   IDLE STATE     │
                    └──────────────────┘

                    ┌──────────────────┐
                    │ DISABLED STATE   │
Any State ─────────→│  • Maintenance   │
(Admin)             │  • All functions │
                    │    disabled      │
                    └──────────────────┘
```

### Thread-Safe State Access

```cpp
class ChargingStateMachine {
private:
    std::atomic<ChargingState> m_currentState;
    std::mutex m_mutex; // For listener vector

public:
    // Thread-safe read
    ChargingState getCurrentState() const {
        return m_currentState.load();
    }

    // Thread-safe write with notification
    bool transitionTo(ChargingState newState) {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Validate and execute transition
        m_currentState.store(newState);
        notifyStateChange();
    }
};
```

---

## 📡 Communication Protocols

### UDP Message Format (ISO 15118 Simplified)

```
┌──────────────────────────────────────┐
│  Message Structure (Binary)          │
├──────────────────────────────────────┤
│  Header (4 bytes)                    │
│  - Magic: 0xABCD                     │
│  - Version: 0x01                     │
│  - Type: Command/Status              │
├──────────────────────────────────────┤
│  Payload (variable)                  │
│  - State: IDLE/CHARGING/etc.         │
│  - Current: float (4 bytes)          │
│  - Voltage: float (4 bytes)          │
│  - Power: float (4 bytes)            │
│  - Timestamp: uint64_t (8 bytes)     │
├──────────────────────────────────────┤
│  Checksum (2 bytes)                  │
│  - CRC16 of header + payload         │
└──────────────────────────────────────┘
```

### HTTP API Response Format

```json
{
  "success": true,
  "state": "CHARGING",
  "data": {
    "current": 16.5,
    "voltage": 230.0,
    "power": 3795.0,
    "energy": 12.5,
    "duration": 1234,
    "relay_enabled": true,
    "wallbox_enabled": true
  },
  "timestamp": "2025-12-13T10:30:45Z"
}
```

---

## 🧪 Testing Architecture

### Dependency Injection for Testability

```cpp
// Production
auto gpio = std::make_unique<BananaPiGpioController>();
auto wallbox = std::make_unique<WallboxController>(
    std::move(gpio), std::move(network)
);

// Testing
auto mockGpio = std::make_unique<MockGpioController>();
auto wallbox = std::make_unique<WallboxController>(
    std::move(mockGpio), std::move(mockNetwork)
);

// Verify GPIO was called
EXPECT_CALL(*mockGpio, digitalWrite(21, HIGH));
wallbox->startCharging();
```

### Unit Test Structure

```cpp
class WallboxControllerTest : public ::testing::Test {
protected:
    std::unique_ptr<MockGpioController> mockGpio;
    std::unique_ptr<MockNetworkCommunicator> mockNetwork;
    std::unique_ptr<WallboxController> controller;

    void SetUp() override {
        mockGpio = std::make_unique<MockGpioController>();
        mockNetwork = std::make_unique<MockNetworkCommunicator>();
        controller = std::make_unique<WallboxController>(
            std::move(mockGpio), std::move(mockNetwork)
        );
    }
};
```

---

## 📚 Class Responsibilities Summary

| Class                      | Primary Responsibility | Design Pattern   | SOLID Principle |
| -------------------------- | ---------------------- | ---------------- | --------------- |
| **WallboxController**      | System orchestration   | Facade, Observer | SRP, DIP        |
| **ChargingStateMachine**   | State management       | State, Observer  | SRP, OCP        |
| **HttpApiServer**          | REST API               | Command, Facade  | SRP, ISP        |
| **IGpioController**        | GPIO abstraction       | Strategy         | DIP, ISP        |
| **INetworkCommunicator**   | Network abstraction    | Strategy         | DIP, ISP        |
| **BananaPiGpioController** | Real GPIO impl.        | Strategy         | LSP             |
| **StubGpioController**     | Mock GPIO impl.        | Strategy         | LSP             |
| **UdpCommunicator**        | UDP networking         | Strategy         | LSP             |
| **Application**            | App lifecycle          | Facade           | SRP             |
| **Configuration**          | Settings management    | Singleton        | SRP             |

**Legend:**

- **SRP**: Single Responsibility Principle
- **OCP**: Open/Closed Principle
- **LSP**: Liskov Substitution Principle
- **ISP**: Interface Segregation Principle
- **DIP**: Dependency Inversion Principle

---

## 🎯 Key Takeaways

1. **Modular Design**: Each class has a single, well-defined responsibility
2. **Interface-Based**: Dependencies on abstractions, not concrete classes
3. **Testable**: Easy to mock and unit test components
4. **Flexible**: Easy to add new features without modifying existing code
5. **Maintainable**: Clear separation of concerns
6. **Thread-Safe**: Designed for concurrent API + Interactive mode
7. **Platform-Independent**: Works on any Pi via GPIO abstraction

---

## 📖 Related Documentation

- [INSTALLATION_GUIDE.md](docs/INSTALLATION_GUIDE.md) - Setup instructions
- [ARCHITECTURE_V3.md](docs/architecture/ARCHITECTURE_V3.md) - System architecture
- [API_REFERENCE.md](docs/api/API_REFERENCE.md) - REST API documentation
- [DUAL_MODE_GUIDE.md](docs/DUAL_MODE_GUIDE.md) - Dual mode operation
- [BUILD_METHODS.md](wallbox-portable-deploy/BUILD_METHODS.md) - Build systems

---

**Document Version:** 1.0  
**Last Updated:** December 13, 2025  
**Author:** Wallbox Development Team  
**Status:** Complete ✅
