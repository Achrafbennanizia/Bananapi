# Wallbox Controller - Class Architecture & Interactions

**Version:** 4.1 (ISO 15118 + CP Signal Reading)  
**Date:** December 13, 2025  
**Purpose:** Complete guide to system architecture, class responsibilities, and interactions

---

## 📋 Table of Contents

1. [System Overview](#system-overview)
2. [Core Classes](#core-classes)
3. [Interface Abstractions](#interface-abstractions)
   - [IGpioController](#igpiocontroller)
   - [INetworkCommunicator](#inetworkcommunicator)
   - [ICpSignalReader](#icpsignalreader) ⭐ NEW
4. [Concrete Implementations](#concrete-implementations)
   - [BananaPiGpioController](#bananapigpiocontroller)
   - [StubGpioController](#stubgpiocontroller)
   - [UdpCommunicator](#udpcommunicator)
   - [HardwareCpSignalReader](#hardwarecpsignalreader) ⭐ NEW
   - [SimulatorCpSignalReader](#simulatorcpsignalreader) ⭐ NEW
   - [CpSignalReaderFactory](#cpsignalreaderfactory) ⭐ NEW
5. [Support Classes](#support-classes)
6. [Class Interaction Diagrams](#class-interaction-diagrams)
7. [Design Patterns Used](#design-patterns-used)
8. [Data Flow](#data-flow)
9. [State Management](#state-management)
10. [CP Signal System](#cp-signal-system) ⭐ NEW

---

## 🎯 System Overview

The Wallbox Controller is a modular, SOLID-principle-based C++ application for managing electric vehicle charging stations. It supports dual-mode operation (API + Interactive) and communicates via UDP with ISO 15118 protocol.

### Architecture Layers

```
┌─────────────────────────────────────────────────────────┐
│                   Application Layer                     │
│        (main_v3.cpp, main_v4.cpp - Entry Points)        │
└─────────────────────────────────────────────────────────┘
                           │
┌─────────────────────────────────────────────────────────┐
│                   Controller Layer                      │
│   WallboxController, SimpleWallboxController            │
│   HttpApiServer, Application                            │
└─────────────────────────────────────────────────────────┘
                           │
┌─────────────────────────────────────────────────────────┐
│                  Business Logic Layer                   │
│   ChargingStateMachine, Configuration                   │
└─────────────────────────────────────────────────────────┘
                           │
┌─────────────────────────────────────────────────────────┐
│               Hardware Abstraction Layer                │
│    IGpioController → BananaPiGpio / StubGpio            │
│    INetworkCommunicator → UdpCommunicator               │
└─────────────────────────────────────────────────────────┘
                           │
┌─────────────────────────────────────────────────────────┐
│                    System Layer                         │
│    Linux GPIO (sysfs), UDP Sockets, ISO 15118           │
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
    OFF,            // 0: No input power supply or severe error
    IDLE,           // 1: No plug connected to charging socket
    CONNECTED,      // 2: ISO message exchange executing
    IDENTIFICATION, // 3: Awaits identification confirmation
    READY,          // 4: Session set up, awaiting power transfer request
    CHARGING,       // 5: Power is being transferred
    STOP,           // 6: Power transfer interrupted, finishing
    FINISHED,       // 7: Charging shut down, plug still connected
    ERROR           // 8: Resettable error, plug must be disconnected
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
OFF → IDLE → CONNECTED → IDENTIFICATION → READY → CHARGING
                                                     ↓
                                                   STOP → FINISHED → IDLE

Any State → ERROR → IDLE → OFF (shutdown)
CHARGING ⇄ READY (pause/resume)
```

#### Key Methods:

```cpp
bool transitionTo(ChargingState newState)  // Execute transition
bool canTransitionTo(ChargingState) const  // Check if valid
bool startCharging()                       // IDLE → CONNECTED → IDENTIFICATION → READY → CHARGING
bool stopCharging()                        // CHARGING/READY → STOP → FINISHED → IDLE
bool pauseCharging()                       // CHARGING → READY
bool resumeCharging()                      // READY → CHARGING
bool resumeCharging()                      // READY → CHARGING
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

### ICpSignalReader

**Location:** `WallboxCtrl/include/ICpSignalReader.h`  
**Purpose:** CP (Control Pilot) signal reading abstraction (IEC 61851-1)

```cpp
enum class CpState : uint8_t {
    STATE_A = 0,  // 12V - No vehicle connected
    STATE_B = 1,  // 9V - Vehicle connected, not ready
    STATE_C = 2,  // 6V - Vehicle ready to charge
    STATE_D = 3,  // 3V - Charging with ventilation
    STATE_E = 4,  // 0V - No power / shutdown
    STATE_F = 5,  // -12V - Error condition
    UNKNOWN = 255
};

class ICpSignalReader {
public:
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual CpState readCpState() = 0;
    virtual std::string getCpStateString(CpState state) const = 0;
    virtual void onStateChange(std::function<void(CpState, CpState)> callback) = 0;
    virtual void startMonitoring() = 0;
    virtual void stopMonitoring() = 0;
};
```

#### Why Interface?

- ✅ **Dual-Mode Support**: Hardware GPIO vs Simulator UDP
- ✅ **IEC 61851-1 Compliance**: Standard CP signal states
- ✅ **ISO 15118 Integration**: Maps to charging states
- ✅ **Strategy Pattern**: Runtime mode selection
- ✅ **Observer Pattern**: State change notifications

#### CP State to ISO 15118 Mapping:

| CP State | Voltage | IEC 61851-1       | ISO 15118 State |
| -------- | ------- | ----------------- | --------------- |
| STATE_A  | +12V    | No vehicle        | IDLE            |
| STATE_B  | +9V     | Vehicle connected | CONNECTED       |
| STATE_C  | +6V     | Ready to charge   | READY           |
| STATE_D  | +3V     | Charging          | CHARGING        |
| STATE_E  | 0V      | Shutdown          | STOP            |
| STATE_F  | -12V    | Error             | ERROR           |

#### Implementations:

1. **HardwareCpSignalReader** - Reads from GPIO pin with ADC
2. **SimulatorCpSignalReader** - Receives from simulator via UDP

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

### HardwareCpSignalReader

**Location:** `WallboxCtrl/src/HardwareCpSignalReader.cpp`  
**Purpose:** Read CP signal from physical GPIO pin (production mode)

```cpp
class HardwareCpSignalReader : public ICpSignalReader {
private:
    std::shared_ptr<IGpioController> m_gpio;
    int m_cpPin;
    CpState m_currentState;
    std::thread m_monitorThread;
    std::atomic<bool> m_monitoring;
    std::vector<std::function<void(CpState, CpState)>> m_callbacks;
    std::mutex m_mutex;

    void monitorLoop();                    // 100ms polling loop
    CpState voltageToState(int voltage);   // IEC 61851-1 mapping
    int readVoltage();                     // ADC voltage reading
    void notifyStateChange(CpState old, CpState new);
};
```

#### IEC 61851-1 Voltage Mapping:

```cpp
CpState voltageToState(int millivolts) {
    if (millivolts > 11000)  return STATE_A;  // 12V - No vehicle
    if (millivolts > 8000)   return STATE_B;  // 9V - Connected
    if (millivolts > 5000)   return STATE_C;  // 6V - Ready
    if (millivolts > 2000)   return STATE_D;  // 3V - Charging
    if (millivolts > 500)    return STATE_E;  // 0V - No power
    if (millivolts < -10000) return STATE_F;  // -12V - Error
    return UNKNOWN;
}
```

#### Monitoring Thread:

- **Polling Rate**: 100ms (10 Hz)
- **State Detection**: Compares voltage to thresholds
- **Change Notification**: Observer callbacks triggered
- **Thread Safety**: Mutex-protected state access

#### Design Patterns:

- **Strategy Pattern**: Concrete implementation of ICpSignalReader
- **Observer Pattern**: State change callbacks
- **Dependency Injection**: GPIO controller injected

---

### SimulatorCpSignalReader

**Location:** `WallboxCtrl/src/SimulatorCpSignalReader.cpp`  
**Purpose:** Receive CP state from simulator via UDP (development mode)

```cpp
class SimulatorCpSignalReader : public ICpSignalReader {
private:
    std::shared_ptr<INetworkCommunicator> m_network;
    CpState m_currentState;
    bool m_initialized;
    std::vector<std::function<void(CpState, CpState)>> m_callbacks;
    std::mutex m_mutex;

    void handleMessage(const std::vector<uint8_t>& data);
    CpState parseStateFromMessage(const std::vector<uint8_t>& data);
    void notifyStateChange(CpState old, CpState new);
};
```

#### UDP Message Protocol:

```
Byte Layout:
┌────────┬──────────┐
│ Byte 0 │ Byte 1   │
├────────┼──────────┤
│ 0x03   │ CP State │
│(Type)  │ (0-6)    │
└────────┴──────────┘

Message Type: 0x03 = CP State Update
CP State Values:
  0 = STATE_A (12V)
  1 = STATE_B (9V)
  2 = STATE_C (6V)
  3 = STATE_D (3V)
  4 = STATE_E (0V)
  5 = STATE_F (-12V)
```

#### Message Parsing:

```cpp
CpState parseStateFromMessage(const std::vector<uint8_t>& data) {
    if (data.size() < 2 || data[0] != 0x03) {
        return CpState::UNKNOWN;  // Invalid message
    }

    uint8_t stateValue = data[1];
    return static_cast<CpState>(stateValue);
}
```

#### Features:

- **Event-Driven**: No polling, reacts to UDP messages
- **Manual Testing**: `setCpState()` for unit tests
- **Asynchronous**: Non-blocking message handling
- **Thread-Safe**: Mutex-protected state updates

#### Design Patterns:

- **Strategy Pattern**: Alternative CP reading strategy
- **Observer Pattern**: Network message callbacks
- **Dependency Injection**: Network communicator injected

---

### CpSignalReaderFactory

**Location:** `WallboxCtrl/src/CpSignalReaderFactory.cpp`  
**Purpose:** Create appropriate CP reader based on operating mode

```cpp
class CpSignalReaderFactory {
public:
    // Create simulator-based reader
    static std::unique_ptr<ICpSignalReader> createSimulatorReader(
        std::shared_ptr<INetworkCommunicator> network
    );

    // Create hardware-based reader
    static std::unique_ptr<ICpSignalReader> createHardwareReader(
        std::shared_ptr<IGpioController> gpio,
        int cpPin = 7
    );

    // Smart factory: mode-based creation
    static std::unique_ptr<ICpSignalReader> create(
        const std::string& mode,
        std::shared_ptr<IGpioController> gpio,
        std::shared_ptr<INetworkCommunicator> network,
        int cpPin = 7
    );
};
```

#### Mode Selection:

```cpp
// Development modes
"simulator", "sim", "development", "dev" → SimulatorCpSignalReader

// Production modes
"hardware", "hw", "production", "prod" → HardwareCpSignalReader
```

#### Usage Example:

```cpp
// In WallboxController::initialize()
const char* envMode = std::getenv("WALLBOX_MODE");
std::string mode = envMode ? envMode : "simulator";

m_cpReader = CpSignalReaderFactory::create(
    mode,                    // "simulator" or "hardware"
    m_gpio,                  // GPIO controller (for hardware)
    m_network,               // Network comm (for simulator)
    Configuration::CP_PIN    // GPIO pin 7
);

m_cpReader->onStateChange([this](CpState old, CpState new) {
    onCpStateChange(old, new);
});

m_cpReader->startMonitoring();
```

#### Design Patterns:

- **Factory Pattern**: Object creation abstraction
- **Strategy Pattern**: Returns interface, not concrete class
- **SOLID**: Open/Closed Principle (extensible)

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

### 5. CP Signal Reading Flow (Dual Mode)

```
┌─────────────────────────────────────────────────────────────┐
│                    CP Signal Reading Flow                   │
└─────────────────────────────────────────────────────────────┘

HARDWARE MODE (Production):      SIMULATOR MODE (Development):

Vehicle CP Signal                          Simulator
    │ (12V/9V/6V/3V)                          │
    ▼                                         │ UDP Message
GPIO Pin 7 (ADC)                              │ [0x03, state]
    │                                         ▼
    ▼                                    INetworkCommunicator
IGpioController                               │
    │                                         ▼
    ▼                                    SimulatorCpSignalReader
HardwareCpSignalReader                        │ parseMessage()
    │ readVoltage()                           │
    │ voltageToState()                        ▼
    │ (IEC 61851-1 mapping)              CpState Update
    ▼                                         │
CpState Update                                │
    │                                         │
    └─────────────┬───────────────────────────┘
                  │
                  ▼
          ICpSignalReader Interface
                  │
                  ▼
          onStateChange() callbacks
                  │
    ┌─────────────┼─────────────┐
    │             │             │
    ▼             ▼             ▼
WallboxController  Logger   API Cache
    │
    ▼
mapCpStateToChargingState()
    │
    ├─→ STATE_A (12V) → IDLE
    ├─→ STATE_B (9V)  → CONNECTED
    ├─→ STATE_C (6V)  → READY
    ├─→ STATE_D (3V)  → CHARGING
    ├─→ STATE_E (0V)  → STOP
    └─→ STATE_F (-12V)→ ERROR
        │
        ▼
ChargingStateMachine
    │ requestTransition()
    ▼
State Machine Execution
    │
    ├─→ Update LEDs
    ├─→ Control Relay
    ├─→ Send UDP status
    └─→ Log transition
```

### 6. Factory Pattern: CP Reader Creation

```
WallboxController::initialize()
    │
    ├─→ Get WALLBOX_MODE from environment
    │   ("simulator" or "production")
    │
    ▼
CpSignalReaderFactory::create(mode, gpio, network, cpPin)
    │
    ├─────────────┬─────────────┐
    │             │             │
mode=="simulator" mode=="hardware"  mode=="unknown"
    │             │             │
    ▼             ▼             ▼
createSimulator   createHardware  throw exception
    │             │
    ▼             ▼
new Simulator    new Hardware
CpSignalReader   CpSignalReader
    │             │
    └──────┬──────┘
           │
           ▼
    unique_ptr<ICpSignalReader>
           │
           ▼
    WallboxController::m_cpReader
           │
           ├─→ initialize()
           ├─→ onStateChange(callback)
           └─→ startMonitoring()
```

### 7. Dual Mode Architecture

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

### Charging State Machine Detailed States (ISO 15118 Standard)

```
┌──────────────────────────────────────────────────────┐
│                    OFF STATE (0)                     │
│  • No input power supply or severe error             │
│  • Communication may have separate supply            │
│  • System shutdown state                             │
│  • Valid transitions: → IDLE, → ERROR                │
└────────────────┬─────────────────────────────────────┘
                 │ Power restored
                 ▼
┌──────────────────────────────────────────────────────┐
│                    IDLE STATE (1)                    │
│  • No plug connected to charging socket              │
│  • Wallbox ready and waiting                         │
│  • Relay OFF                                         │
│  • Valid transitions: → CONNECTED, → OFF, → ERROR    │
└────────────────┬─────────────────────────────────────┘
                 │ Vehicle plug connected
                 ▼
┌──────────────────────────────────────────────────────┐
│                 CONNECTED STATE (2)                  │
│  • ISO 15118 message exchange executing              │
│  • Collecting charging session relevant data         │
│  • Establishing communication protocol               │
│  • Relay still OFF                                   │
│  • Valid transitions: → IDENTIFICATION, → IDLE, → ERROR│
└────────────────┬─────────────────────────────────────┘
                 │ Protocol established
                 ▼
┌──────────────────────────────────────────────────────┐
│              IDENTIFICATION STATE (3)                │
│  • Stack awaits identification confirmation          │
│  • Verifying vehicle credentials                     │
│  • Authentication in progress                        │
│  • Valid transitions: → READY, → IDLE, → ERROR       │
└────────────────┬─────────────────────────────────────┘
                 │ Identification confirmed
                 ▼
┌──────────────────────────────────────────────────────┐
│                  READY STATE (4)                     │
│  • Charging session is set up                        │
│  • Vehicle not yet requested power transfer          │
│  • Waiting for pilot pin signal                      │
│  • Relay ready but OFF                               │
│  • Valid transitions: → CHARGING, → STOP, → IDLE, → ERROR│
└────────────────┬─────────────────────────────────────┘
                 │ Power transfer requested
                 ▼
┌──────────────────────────────────────────────────────┐
│                 CHARGING STATE (5)                   │
│  • Power is being actively transferred               │
│  • Relay ON                                          │
│  • Monitoring current/voltage (may be zero)          │
│  • ISO 15118 communication active                    │
│  • Valid transitions: → READY, → STOP, → ERROR       │
└──────┬──────────────────────────────────────┬────────┘
       │ Pause (to READY)                      │ Stop requested
       ▼                                       ▼
┌──────────────────┐                  ┌──────────────────┐
│  READY STATE (4) │                  │  STOP STATE (6)  │
│  • Paused but    │                  │  • Power transfer│
│    session alive │                  │    interrupted   │
│  • Can resume    │                  │  • Session       │
│  • Relay OFF     │                  │    finishing     │
└──────┬───────────┘                  │  • Relay OFF     │
       │ Resume                        └────────┬─────────┘
       └────────────────┬──────────────────────┘
                        │
                        ▼
                ┌──────────────────┐
                │ FINISHED STATE(7)│
                │  • Charging done │
                │  • Plug still    │
                │    connected     │
                │  • Session closed│
                └────────┬─────────┘
                         │ Unplug
                         ▼
┌──────────────────────────────────────────────────────┐
│                   IDLE STATE (1)                     │
│  • Ready for next session                            │
└──────────────────────────────────────────────────────┘

                    ┌──────────────────┐
                    │  ERROR STATE (8) │
                    │  • Resettable    │
Any State ─────────→│    error         │
                    │  • Relay OFF     │
                    │  • Plug must be  │
                    │    disconnected  │
                    │    to reset      │
                    └────────┬─────────┘
                             │ Reset
                             ▼
                    ┌──────────────────┐
                    │   IDLE STATE (1) │
                    └──────────────────┘
```

**State Numbers**: Following ISO 15118 enIsoChargingState enumeration (0-8)

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

| Class                          | Primary Responsibility | Design Pattern     | SOLID Principle |
| ------------------------------ | ---------------------- | ------------------ | --------------- |
| **WallboxController**          | System orchestration   | Facade, Observer   | SRP, DIP        |
| **ChargingStateMachine**       | State management       | State, Observer    | SRP, OCP        |
| **HttpApiServer**              | REST API               | Command, Facade    | SRP, ISP        |
| **IGpioController**            | GPIO abstraction       | Strategy           | DIP, ISP        |
| **INetworkCommunicator**       | Network abstraction    | Strategy           | DIP, ISP        |
| **ICpSignalReader** ⭐         | CP signal abstraction  | Strategy, Observer | DIP, ISP        |
| **BananaPiGpioController**     | Real GPIO impl.        | Strategy           | LSP             |
| **StubGpioController**         | Mock GPIO impl.        | Strategy           | LSP             |
| **UdpCommunicator**            | UDP networking         | Strategy           | LSP             |
| **HardwareCpSignalReader** ⭐  | CP from GPIO pins      | Strategy           | LSP             |
| **SimulatorCpSignalReader** ⭐ | CP from UDP simulator  | Strategy           | LSP             |
| **CpSignalReaderFactory** ⭐   | CP reader creation     | Factory            | OCP, DIP        |
| **Application**                | App lifecycle          | Facade             | SRP             |
| **Configuration**              | Settings management    | Singleton          | SRP             |

**Legend:**

- **SRP**: Single Responsibility Principle
- **OCP**: Open/Closed Principle
- **LSP**: Liskov Substitution Principle
- **ISP**: Interface Segregation Principle
- **DIP**: Dependency Inversion Principle

---

## 🔌 CP Signal System

### Overview

The CP (Control Pilot) Signal System enables the wallbox to detect vehicle connection status and charging readiness according to **IEC 61851-1** standard. The system operates in two modes:

1. **Hardware Mode**: Reads CP signal from GPIO pin 7 (ADC-capable) in production
2. **Simulator Mode**: Receives CP state from simulator via UDP in development

### Architecture

```
┌───────────────────────────────────────────────────────┐
│              ICpSignalReader Interface                │
│  (Strategy Pattern - Abstract CP signal reading)      │
└───────────────────┬───────────────────────────────────┘
                    │
        ┌───────────┴───────────┐
        │                       │
        ▼                       ▼
┌──────────────────┐    ┌──────────────────┐
│   Hardware       │    │   Simulator      │
│  CpSignalReader  │    │  CpSignalReader  │
│                  │    │                  │
│ Uses: GPIO       │    │ Uses: Network    │
│ Mode: Production │    │ Mode: Dev        │
└──────────────────┘    └──────────────────┘
```

### IEC 61851-1 State Mapping

| CP Voltage | CP State | Meaning                | ISO 15118 State |
| ---------- | -------- | ---------------------- | --------------- |
| +12V       | STATE_A  | No vehicle             | IDLE            |
| +9V        | STATE_B  | Vehicle connected      | CONNECTED       |
| +6V        | STATE_C  | Ready to charge        | READY           |
| +3V        | STATE_D  | Charging + ventilation | CHARGING        |
| 0V         | STATE_E  | No power               | STOP            |
| -12V       | STATE_F  | Error condition        | ERROR           |

### State Transition Flow

```
Vehicle Connection Sequence:

STATE_A (12V)     →    IDLE
   ↓ (Vehicle plugged in)
STATE_B (9V)      →    CONNECTED
   ↓ (Vehicle ready)
STATE_C (6V)      →    READY
   ↓ (Charging authorized)
STATE_D (3V)      →    CHARGING
   ↓ (Charging complete)
STATE_C (6V)      →    STOP
   ↓ (Vehicle disconnected)
STATE_A (12V)     →    IDLE

Error Condition:
STATE_F (-12V)    →    ERROR
```

### Factory Pattern Usage

```cpp
// Mode determined from environment variable
const char* mode = std::getenv("WALLBOX_MODE");

// Factory creates appropriate reader
auto cpReader = CpSignalReaderFactory::create(
    mode,           // "simulator" or "hardware"
    gpio,           // For hardware mode
    network,        // For simulator mode
    7               // GPIO pin number
);

// Interface used polymorphically
cpReader->initialize();
cpReader->onStateChange(callback);
cpReader->startMonitoring();
```

### Observer Pattern Implementation

```cpp
// WallboxController registers as observer
m_cpReader->onStateChange([this](CpState oldState, CpState newState) {
    std::cout << "CP changed: "
              << m_cpReader->getCpStateString(oldState)
              << " → "
              << m_cpReader->getCpStateString(newState)
              << std::endl;

    // Map CP state to charging state
    mapCpStateToChargingState(newState);
});
```

### Hardware Mode Details

**Hardware:** Banana Pi GPIO with ADC simulation  
**Pin:** GPIO 7 (configurable)  
**Polling:** 100ms (10 Hz)  
**Thread:** Dedicated monitoring thread  
**Voltage Reading:** Simulated ADC (to be replaced with real ADC)

```cpp
// Voltage thresholds (IEC 61851-1)
if (voltage > 11000mV)  return STATE_A;   // 12V
if (voltage > 8000mV)   return STATE_B;   // 9V
if (voltage > 5000mV)   return STATE_C;   // 6V
if (voltage > 2000mV)   return STATE_D;   // 3V
if (voltage > 500mV)    return STATE_E;   // 0V
if (voltage < -10000mV) return STATE_F;   // -12V
```

### Simulator Mode Details

**Protocol:** UDP on ports 50010/50011  
**Message Format:** `[0x03, CP_STATE]`  
**Async:** Event-driven (no polling)  
**Thread:** Uses network receive thread

```cpp
// Simulator sends CP state updates:
uint8_t message[2] = {0x03, CP_STATE_B};  // Vehicle connected
network->send(message, 2);

// SimulatorCpSignalReader receives and parses
CpState state = parseStateFromMessage(message);
// → STATE_B (9V - Vehicle connected)
```

### Design Patterns Summary

1. **Strategy Pattern**: Different CP reading strategies (hardware vs simulator)
2. **Factory Pattern**: Mode-based reader instantiation
3. **Observer Pattern**: State change notifications
4. **Dependency Injection**: GPIO and network dependencies injected
5. **Interface Segregation**: Minimal, focused interface

### SOLID Principles Applied

- **SRP**: Each reader has single responsibility (read CP)
- **OCP**: Extensible (add new readers without modification)
- **LSP**: All readers substitutable via interface
- **ISP**: Interface contains only CP-related methods
- **DIP**: Depends on abstractions (IGpio, INetwork)

### Configuration

```json
{
  "cp_pin": 7,
  "mode": "simulator"
}
```

Environment variable:

```bash
export WALLBOX_MODE=simulator  # or "production"
```

### Testing

```cpp
// Unit test: Hardware reader voltage mapping
TEST(HardwareCpReader, VoltageMapping) {
    EXPECT_EQ(reader.voltageToState(12000), CpState::STATE_A);
    EXPECT_EQ(reader.voltageToState(9000), CpState::STATE_B);
}

// Unit test: Simulator message parsing
TEST(SimulatorCpReader, MessageParsing) {
    std::vector<uint8_t> msg = {0x03, 0x01};  // STATE_B
    EXPECT_EQ(reader.parseStateFromMessage(msg), CpState::STATE_B);
}

// Integration test: State change callback
TEST(CpReader, StateChangeNotification) {
    bool called = false;
    reader.onStateChange([&](auto old, auto new) { called = true; });
    reader.setCpState(CpState::STATE_B);
    EXPECT_TRUE(called);
}
```

### Future Enhancements

1. **Real ADC Integration**: Replace simulated voltage with actual ADC chip
2. **PWM Duty Cycle**: Read PWM signal for current limit
3. **Debouncing**: Filter noisy CP signals
4. **Diagnostics**: Log voltage anomalies for troubleshooting
5. **Multi-Connector**: Support multiple CP readers for multi-port chargers

---

## 🎯 Key Takeaways

1. **Modular Design**: Each class has a single, well-defined responsibility
2. **Interface-Based**: Dependencies on abstractions, not concrete classes
3. **Testable**: Easy to mock and unit test components
4. **Flexible**: Easy to add new features without modifying existing code
5. **Maintainable**: Clear separation of concerns
6. **Thread-Safe**: Designed for concurrent API + Interactive mode
7. **Platform-Independent**: Works on any Pi via GPIO abstraction
8. **Dual-Mode CP Reading**: ⭐ Hardware (GPIO) and simulator (UDP) support
9. **IEC 61851-1 Compliant**: ⭐ Standard CP signal interpretation
10. **ISO 15118 Integration**: ⭐ CP states mapped to charging states

---

## 📖 Related Documentation

- [CP_SIGNAL_IMPLEMENTATION.md](docs/CP_SIGNAL_IMPLEMENTATION.md) - ⭐ CP signal system details
- [INSTALLATION_GUIDE.md](docs/INSTALLATION_GUIDE.md) - Setup instructions
- [ARCHITECTURE_V3.md](docs/architecture/ARCHITECTURE_V3.md) - System architecture
- [API_REFERENCE.md](docs/api/API_REFERENCE.md) - REST API documentation
- [DUAL_MODE_GUIDE.md](docs/DUAL_MODE_GUIDE.md) - Dual mode operation
- [BUILD_METHODS.md](wallbox-portable-deploy/BUILD_METHODS.md) - Build systems

---

**Document Version:** 1.0  
**Last Updated:** December 26, 2025  
**Author:** Achraf Bennani  
**Status:** Complete ✅
