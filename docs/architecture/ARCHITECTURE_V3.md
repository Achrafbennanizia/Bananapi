# Wallbox Controller v4.0 - Modern Architecture

**Last Updated**: December 14, 2025  
**Version**: 4.0 (Latest)
**Status**: Production Ready

## 🎯 Executive Summary

The Wallbox Controller v4.0 implements a modern, industry-standard architecture with:

- **HTTP REST API** - Full remote control via port 8080
- **SOLID principles** throughout the codebase
- **7 design patterns** working in harmony
- **Clean architecture** with clear boundaries
- **CP Signal System** - IEC 61851-1 compliant (Hardware + Simulator)
- **State machine** with enforced transitions: `idle → ready → charging`
- **Relay validation** for safety-critical operations
- **Dual mode operation** - Interactive terminal or HTTP API
- **UDP Communication** - ISO 15118 stack messaging
- **Independent simulator** with feedback display

---

## 🏗️ System Architecture Overview

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        User Layer                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ React WebApp │  │   Simulator  │  │ Hardware Pins│      │
│  │ (Port 3000)  │  │   (UDP)      │  │  (GPIO)      │      │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘      │
└─────────┼──────────────────┼──────────────────┼─────────────┘
          │                  │                  │
          ▼                  ▼                  ▼
┌─────────────────────────────────────────────────────────────┐
│                     Interface Layer                          │
│  ┌─────────────┐    ┌──────────────┐   ┌───────────────┐   │
│  │HTTP API     │    │UDP Network   │   │GPIO Interface │   │
│  │(Port 8080)  │    │(50010/50011) │   │(IGpioCtrl)    │   │
│  └─────┬───────┘    └──────┬───────┘   └───────┬───────┘   │
└────────┼────────────────────┼─────────────────────┼─────────┘
         │                    │                     │
         └──────────┬─────────┴──────────┬──────────┘
                    ▼                    ▼
         ┌────────────────────────────────────────┐
         │      Application Controller             │
         │  • Lifecycle Management                 │
         │  • Component Orchestration              │
         │  • Signal Handling                      │
         └─────────────┬──────────────────────────┘
                       │
         ┌─────────────┴──────────────────────────┐
         │                                         │
         ▼                                         ▼
┌────────────────────┐                  ┌──────────────────┐
│ WallboxController  │◄─────────────────┤ Configuration    │
│  (Facade)          │                  │  (Singleton)     │
└─────┬──────────────┘                  └──────────────────┘
      │
      ├─────────────────┬──────────────────┬────────────────┐
      ▼                 ▼                  ▼                ▼
┌──────────┐   ┌─────────────┐   ┌──────────────┐  ┌──────────┐
│ State    │   │   GPIO      │   │   Network    │  │   LED    │
│ Machine  │   │ Controller  │   │Communicator  │  │Controller│
│(State)   │   │  (Strategy) │   │(Strategy)    │  │          │
└──────────┘   └─────────────┘   └──────────────┘  └──────────┘
     │
     │ (Observer Pattern)
     ▼
┌────────────────────────────────┐
│  State Change Listeners        │
│  • Update LEDs                 │
│  • Send Network Status         │
│  • Log Transitions             │
└────────────────────────────────┘
```

---

## 📊 Component Architecture

### Core Components

```
Application
    ├── Configuration (Singleton)
    │   ├── System Settings
    │   ├── Pin Definitions
    │   └── Network Ports
    │
    ├── WallboxController (Facade)
    │   ├── ChargingStateMachine (State Pattern)
    │   ├── IGpioController (Strategy Pattern)
    │   │   ├── BananaPiGpioController
    │   │   └── StubGpioController
    │   ├── INetworkCommunicator (Strategy Pattern)
    │   │   └── UdpCommunicator
    │   └── LED Controller
    │
    ├── HttpApiServer
    │   └── ApiController (MVC Controller)
    │
    └── GpioFactory (Factory Pattern)
```

---

## 🔄 State Machine Architecture

### Charging States & Transitions

```
                    ┌─────────┐
                    │  START  │
                    └────┬────┘
                         │
                         ▼
                    ┌─────────┐
          ┌────────▶│  IDLE   │◄────────┐
          │         └────┬────┘         │
          │              │              │
          │              │ (Relay ON)   │
          │              ▼              │
          │         ┌─────────┐         │
          │         │  READY  │         │
          │         └────┬────┘         │
          │              │              │
          │              │ (Ready cmd)  │
          │              ▼              │
          │         ┌──────────┐        │
          │         │ CHARGING │        │
          │         └────┬─────┘        │
          │              │              │
          │              ├─────────┐    │
          │              │         │    │
          │         (pause)     (stop)  │
          │              │         │    │
          │              ▼         └────┘
          │         ┌─────────┐
          └─────────┤ PAUSED  │
                    └─────────┘

Rules:
• idle → ready: Requires relay ON
• ready → charging: Requires relay ON + previous state ready
• Any state → stop: Always allowed
• Any state → idle: Always allowed
```

### State Validation Logic

```cpp
// In WallboxController::processNetworkMessage()

case enIsoChargingState::ready:
    if (!m_relayEnabled) {
        // ❌ Cannot go to READY: Relay must be ON first
        reject();
    } else if (currentWallboxState == ChargingState::IDLE) {
        // ✓ Valid transition
        accept();
    }
    break;

case enIsoChargingState::charging:
    if (!m_relayEnabled) {
        // ❌ Cannot start charging: Relay must be ON
        reject();
    } else if (lastState == enIsoChargingState::ready) {
        // ✓ Valid: idle → ready → charging
        startCharging();
    } else {
        // ❌ Must follow correct sequence
        reject();
    }
    break;
```

---

## 🎨 Design Patterns in Detail

// Organized by domain
setupHealthEndpoints()
setupStatusEndpoints()
setupChargingEndpoints()
setupWallboxEndpoints()

````

**Benefits:**

- API logic separated from business logic
- Easy to find endpoints
- Testable in isolation

### 4. Application Controller - Application

```cpp
// Manages entire lifecycle

### 1. **Singleton Pattern** - Configuration

```cpp
// Thread-safe, lazy initialization
Configuration& config = Configuration::getInstance();

// Environment-driven configuration
config.loadFromEnvironment();

// Type-safe access
int port = config.getApiPort();
Mode mode = config.getMode();
```

**Benefits:**
- Single source of truth
- No global variables
- Easy testing (can mock getInstance)

---

### 2. **Factory Pattern** - GpioFactory

```cpp
// Simple creation
auto gpio = GpioFactory::create("stub");

// Mode-specific creation
auto devGpio = GpioFactory::createForDevelopment();
auto prodGpio = GpioFactory::createForProduction();
```

**Benefits:**
- Encapsulates creation logic
- Easy to add new types
- Returns interfaces, not concrete classes

---

### 3. **Strategy Pattern** - GPIO & Network

```cpp
// Strategy interface
class IGpioController {
    virtual bool initialize() = 0;
    virtual void digitalWrite(int pin, PinValue value) = 0;
};

// Concrete strategies
class BananaPiGpioController : public IGpioController { /* ... */ };
class StubGpioController : public IGpioController { /* ... */ };

// Context uses strategy (runtime selection)
std::unique_ptr<IGpioController> gpio = GpioFactory::create(type);
```

**Benefits:**
- Interchangeable algorithms
- Runtime behavior selection
- Easy to test with stubs

---

### 4. **State Pattern** - Charging State Machine

```cpp
enum class ChargingState {
    IDLE, PREPARING, CHARGING, PAUSED, FINISHING, ERROR, DISABLED
};

class ChargingStateMachine {
    ChargingState m_currentState;

    bool transitionTo(ChargingState newState, const std::string &reason);
    bool canTransitionTo(ChargingState newState) const;
    bool isValidTransition(ChargingState from, ChargingState to) const;
};
```

**State Transition Rules:**
```
IDLE → PREPARING (relay must be ON)
PREPARING → CHARGING (previous state must be PREPARING)
CHARGING → PAUSED (user request)
PAUSED → CHARGING (resume)
Any state → IDLE (always allowed)
Any state → STOP (always allowed)
```

**Benefits:**
- Encapsulates state-specific behavior
- Enforces valid transitions
- Clear state lifecycle

---

### 5. **Observer Pattern** - State Change Notifications

```cpp
// Observer callback
using StateChangeCallback = std::function<void(
    ChargingState oldState,
    ChargingState newState,
    const std::string &reason
)>;

// Subject (Observable)
class ChargingStateMachine {
    std::vector<StateChangeCallback> m_listeners;

    void addStateChangeListener(StateChangeCallback callback);
    void notifyStateChange(/* ... */);
};

// Observer registration
m_stateMachine->addStateChangeListener(
    [this](ChargingState old, ChargingState new, const std::string &reason) {
        onStateChange(old, new, reason);  // Update LEDs, send status, etc.
    }
);
```

**Benefits:**
- Decouples state machine from observers
- Multiple listeners possible
- Flexible notification system

---

### 6. **Facade Pattern** - WallboxController

```cpp
class WallboxController {
public:
    // Simple facade methods hide complexity
    bool startCharging();
    bool stopCharging();
    bool pauseCharging();

private:
    // Complex subsystems hidden behind facade
    std::unique_ptr<IGpioController> m_gpio;
    std::unique_ptr<INetworkCommunicator> m_network;
    std::unique_ptr<ChargingStateMachine> m_stateMachine;

    // Internal coordination
    void setupGpio();
    void updateLeds();
    void sendStatusToSimulator();
};
```

**Benefits:**
- Simplifies complex subsystem interactions
- Single entry point for clients
- Hides implementation details

---

### 7. **Dependency Injection Pattern** - Throughout

```cpp
// Constructor injection (preferred)
WallboxController(
    std::unique_ptr<IGpioController> gpio,
    std::unique_ptr<INetworkCommunicator> network
);

// Usage
auto gpio = GpioFactory::create("stub");
auto network = std::make_unique<UdpCommunicator>(50010, 50011);
auto controller = std::make_unique<WallboxController>(
    std::move(gpio),
    std::move(network)
);
```

**Benefits:**
- Testability (inject mocks/stubs)
- Flexibility (swap implementations)
- Loose coupling (no hard dependencies)

---

## 💡 SOLID Principles Application

### Single Responsibility Principle (SRP)

| Class | Single Responsibility |
|-------|----------------------|
| `Configuration` | Manage configuration settings only |
| `GpioFactory` | Create GPIO controllers only |
| `ChargingStateMachine` | Manage state transitions only |
| `WallboxController` | Coordinate subsystems only |
| `ApiController` | Handle API routing only |
| `Application` | Manage application lifecycle only |

### Open/Closed Principle (OCP)

✅ **Open for extension, closed for modification:**

```cpp
// Adding new GPIO type - NO modification to existing code
class NewGpioController : public IGpioController {
    // Implement interface
};

// Just add to factory
if (type == "newtype") {
    return std::make_unique<NewGpioController>();
}
```

### Liskov Substitution Principle (LSP)

✅ **All implementations are interchangeable:**

```cpp
// Any IGpioController works identically
std::unique_ptr<IGpioController> gpio;

gpio = std::make_unique<BananaPiGpioController>();  // Production
gpio = std::make_unique<StubGpioController>();      // Testing

// Both honor the interface contract
```

### Interface Segregation Principle (ISP)

✅ **Focused, minimal interfaces:**

```cpp
// IGpioController - Only GPIO operations
class IGpioController {
    virtual bool initialize() = 0;
    virtual void digitalWrite(int pin, PinValue value) = 0;
    virtual PinValue digitalRead(int pin) = 0;
    virtual void shutdown() = 0;
};

// INetworkCommunicator - Only network operations
class INetworkCommunicator {
    virtual bool connect() = 0;
    virtual void send(const std::vector<uint8_t> &data) = 0;
    virtual void startReceiving(MessageCallback callback) = 0;
};
```

### Dependency Inversion Principle (DIP)

✅ **Depend on abstractions, not concretions:**

```cpp
class WallboxController {
private:
    // Depends on INTERFACES (abstractions)
    std::unique_ptr<IGpioController> m_gpio;           // NOT BananaPiGpioController
    std::unique_ptr<INetworkCommunicator> m_network;   // NOT UdpCommunicator
};
```

---

## 🔄 Communication Flow

### Simulator → Wallbox Communication

```
┌──────────────┐                        ┌────────────────┐
│  Simulator   │                        │   Wallbox      │
│              │                        │                │
│  State:      │   UDP (Port 50010)     │   Receives:    │
│  - idle      │────────────────────────▶│   - idle cmd   │
│  - ready     │                        │   - ready cmd  │
│  - charge    │                        │   - charge cmd │
│  - stop      │                        │   - stop cmd   │
│              │                        │                │
│  Contactor:  │                        │   Validates:   │
│  - on/off    │                        │   - Relay ON?  │
│              │                        │   - Enabled?   │
│              │                        │   - Valid seq? │
│              │   UDP (Port 50011)     │                │
│  Receives:   │◀────────────────────────   Sends:       │
│  feedback    │                        │   status       │
│              │                        │                │
└──────────────┘                        └────────────────┘

Feedback Messages (Wallbox → Simulator):
✅ "Wallbox ENABLED - Ready to accept commands"
🔴 "Wallbox DISABLED - Commands may be rejected"
⚡ "Main contactor activated"
🔌 "Main contactor deactivated"
❌ "Cannot go to READY: Relay must be ON first"
❌ "Cannot start charging: Must go idle → ready → charge"
```

### React App → Wallbox Communication

```
┌──────────────┐                        ┌────────────────┐
│ React WebApp │                        │   Wallbox      │
│ (Port 3000)  │                        │   API Server   │
│              │   HTTP (Port 8080)     │   (Port 8080)  │
│              │────────────────────────▶│                │
│  GET /api/status                      │   Returns:     │
│                                       │   - state      │
│              │◀────────────────────────   - relay      │
│  Response    │                        │   - charging   │
│              │                        │   - enabled    │
│              │                        │                │
│  POST /api/charging/start             │   Executes:    │
│  POST /api/charging/stop              │   - Commands   │
│  POST /api/charging/pause             │   - Validates  │
│  POST /api/charging/resume            │   - Updates    │
│              │                        │                │
└──────────────┘                        └────────────────┘
```

---

## 📊 Metrics & Comparison

### Code Quality Metrics

| Metric | v2.0 | v3.0 | Improvement |
|--------|------|------|-------------|
| main.cpp lines | 288 | 69 | ↓ 76% |
| Classes | 7 | 11 | ↑ 57% (better separation) |
| Design patterns | 3 | 7 | ↑ 133% |
| SOLID compliance | Partial | Full | ✅ 100% |
| Testability | Moderate | High | ✅ |
| State validation | None | Full | ✅ |
| Relay safety checks | Partial | Full | ✅ |

### Architecture Improvements

| Feature | v2.0 | v3.0 |
|---------|------|------|
| Configuration management | Scattered | Centralized (Singleton) |
| State transitions | Any → Any | Enforced sequence |
| Relay validation | None | Required for ready/charging |
| Simulator feedback | None | Full visibility |
| Enable/Disable logic | In simulator | In wallbox only |
| Command validation | Minimal | Comprehensive |

---

## 🧪 Testing Benefits

### Unit Testing Example

```cpp
// Mock dependencies
MockGpioController mockGpio;
MockNetworkCommunicator mockNetwork;

// Inject mocks
WallboxController controller(
    std::make_unique<MockGpioController>(),
    std::make_unique<MockNetworkCommunicator>()
);

// Test without hardware
controller.startCharging();
ASSERT_EQ(controller.getCurrentState(), ChargingState::CHARGING);
ASSERT_TRUE(mockGpio.wasDigitalWriteCalled());
```

### Integration Testing

```cpp
// Use stub GPIO for integration tests
auto gpio = GpioFactory::create("stub");
auto network = std::make_unique<UdpCommunicator>(50010, 50011);

WallboxController controller(std::move(gpio), std::move(network));

// Test full flow without hardware
controller.initialize();
controller.startCharging();
// Verify state, network messages, etc.
```

---

## 🚀 Future Extensibility

### Adding New Components

**New GPIO Type:**
```cpp
// 1. Create implementation
class RaspberryPiGpioController : public IGpioController { /* ... */ };

// 2. Add to factory (1 line)
if (type == "raspberrypi") return std::make_unique<RaspberryPiGpioController>();
```

**New Network Protocol:**
```cpp
// 1. Create implementation
class TcpCommunicator : public INetworkCommunicator { /* ... */ };

// 2. Inject via constructor
auto network = std::make_unique<TcpCommunicator>(port);
auto controller = std::make_unique<WallboxController>(gpio, std::move(network));
```

**New Charging State:**
```cpp
// 1. Add to enum
enum class ChargingState {
    IDLE, PREPARING, CHARGING, PAUSED, FINISHING, ERROR, DISABLED,
    SCHEDULED  // NEW STATE
};

// 2. Add validation rules
bool ChargingStateMachine::isValidTransition(ChargingState from, ChargingState to) {
    if (to == ChargingState::SCHEDULED && from == ChargingState::IDLE) {
        return true;  // Can schedule from idle
    }
    // ... existing rules
}
```

---

## 📝 Best Practices Implemented

### 1. **Clear Separation of Concerns**
- Each class has one responsibility
- Loose coupling via interfaces
- High cohesion within classes

### 2. **Defensive Programming**
- State validation at every transition
- Relay safety checks
- Enable/disable enforcement
- Comprehensive error messages

### 3. **Observable Behavior**
- State changes trigger notifications
- Feedback messages for invalid commands
- Status updates sent automatically

### 4. **Flexible Configuration**
- Environment variables
- Singleton pattern
- Runtime mode selection

### 5. **Comprehensive Testing Support**
- Dependency injection
- Mock/stub implementations
- Interface-based design

---

## 🎯 Recommended Usage

### Development Mode
```bash
WALLBOX_MODE=dev ./wallbox_control_v3
# Uses StubGpioController (no hardware needed)
```

### Production Mode
```bash
WALLBOX_MODE=prod ./wallbox_control_v3
# Uses BananaPiGpioController (real hardware)
```

### With React App
```bash
# Terminal 1: Start wallbox
./wallbox_control_v3

# Terminal 2: Start React app
cd web/react-app && npm start

# Terminal 3: Start simulator
./simulator
```

---

## 📚 Documentation References

- `SOLID_DESIGN_PATTERNS.md` - Detailed pattern documentation
- `SIMULATOR_INDEPENDENCE.md` - Simulator architecture
- `REMOVAL_SUMMARY.md` - Cleanup documentation
- `CHECKLIST.md` - Implementation verification

---

## 🏆 Summary

Version 3.0 achieves production-ready quality through:

✅ **7 Design Patterns** working in harmony
✅ **Full SOLID Compliance** throughout codebase
✅ **State Machine** with enforced transitions
✅ **Relay Safety** validation
✅ **Independent Simulator** with feedback
✅ **Clean Architecture** with clear boundaries
✅ **High Testability** via dependency injection
✅ **Easy Extensibility** for future features

**Result:** Professional, maintainable, production-ready embedded system.
````
