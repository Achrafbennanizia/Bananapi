# SOLID Principles and Design Patterns in Wallbox Controller

**Date**: December 10, 2025  
**Status**: ✅ Implemented

## Overview

The Wallbox Controller project implements SOLID principles and multiple design patterns to ensure maintainability, testability, and extensibility.

---

## SOLID Principles Implementation

### 1. Single Responsibility Principle (SRP)

**"A class should have only one reason to change"**

✅ **Implementation:**

| Class                  | Single Responsibility                            |
| ---------------------- | ------------------------------------------------ |
| `WallboxController`    | Coordinates subsystems and orchestrates workflow |
| `ChargingStateMachine` | Manages charging state transitions only          |
| `IGpioController`      | GPIO operations only                             |
| `INetworkCommunicator` | Network communication only                       |
| `HttpApiServer`        | HTTP API endpoints and routing only              |
| `Configuration`        | System configuration and constants               |

**Example:**

```cpp
// ChargingStateMachine - Only manages states
class ChargingStateMachine {
    bool startCharging(const std::string &reason);
    bool stopCharging(const std::string &reason);
    ChargingState getCurrentState() const;
    // No GPIO, no network, no LEDs - just states!
};
```

---

### 2. Open/Closed Principle (OCP)

**"Open for extension, closed for modification"**

✅ **Implementation:**

**Interface-based extensibility:**

```cpp
// Add new GPIO implementation without modifying existing code
class IGpioController {
    virtual bool initialize() = 0;
    virtual void digitalWrite(int pin, PinValue value) = 0;
};

// Existing implementations
class BananaPiGpioController : public IGpioController { /* ... */ };
class StubGpioController : public IGpioController { /* ... */ };

// NEW implementation - no changes to WallboxController needed!
class RaspberryPiGpioController : public IGpioController { /* ... */ };
```

**Strategy Pattern enables extension:**

```cpp
// WallboxController accepts any IGpioController implementation
WallboxController(std::unique_ptr<IGpioController> gpio,
                  std::unique_ptr<INetworkCommunicator> network);
```

---

### 3. Liskov Substitution Principle (LSP)

**"Derived classes must be substitutable for their base classes"**

✅ **Implementation:**

```cpp
// Any IGpioController can be used interchangeably
std::unique_ptr<IGpioController> gpio;

// Runtime - BananaPi hardware
gpio = std::make_unique<BananaPiGpioController>();

// Testing - Stub
gpio = std::make_unique<StubGpioController>();

// Both work identically in WallboxController!
```

**All implementations honor the interface contract:**

- `initialize()` returns bool for success/failure
- `digitalWrite()` sets pin values
- No unexpected side effects

---

### 4. Interface Segregation Principle (ISP)

**"Clients should not depend on interfaces they don't use"**

✅ **Implementation:**

**Focused interfaces:**

```cpp
// IGpioController - Only GPIO operations
class IGpioController {
    virtual bool initialize() = 0;
    virtual bool setPinMode(int pin, PinMode mode) = 0;
    virtual void digitalWrite(int pin, PinValue value) = 0;
    virtual PinValue digitalRead(int pin) = 0;
    virtual void shutdown() = 0;
};

// INetworkCommunicator - Only network operations
class INetworkCommunicator {
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual void send(const std::vector<uint8_t> &data) = 0;
    virtual void startReceiving(MessageCallback callback) = 0;
    virtual void stopReceiving() = 0;
};
```

**No "god interface" with unrelated methods!**

---

### 5. Dependency Inversion Principle (DIP)

**"Depend on abstractions, not concretions"**

✅ **Implementation:**

**High-level depends on abstractions:**

```cpp
class WallboxController {
private:
    // Depends on INTERFACES (abstractions)
    std::unique_ptr<IGpioController> m_gpio;           // NOT BananaPiGpioController
    std::unique_ptr<INetworkCommunicator> m_network;   // NOT UdpCommunicator
    std::unique_ptr<ChargingStateMachine> m_stateMachine;
};
```

**Dependency Injection via constructor:**

```cpp
// Dependencies injected from outside
WallboxController(std::unique_ptr<IGpioController> gpio,
                  std::unique_ptr<INetworkCommunicator> network);

// Usage (in main.cpp)
auto gpio = std::make_unique<BananaPiGpioController>();
auto network = std::make_unique<UdpCommunicator>(50010, 50011);
auto controller = std::make_unique<WallboxController>(
    std::move(gpio),
    std::move(network)
);
```

---

## Design Patterns Implementation

### 1. Strategy Pattern

**"Define a family of algorithms, encapsulate each one, and make them interchangeable"**

✅ **Used for:**

- **GPIO strategies**: `BananaPiGpioController`, `StubGpioController`
- **Network strategies**: `UdpCommunicator` (can add TCP, WebSocket, etc.)

```cpp
// Strategy interface
class IGpioController { /* ... */ };

// Concrete strategies
class BananaPiGpioController : public IGpioController { /* ... */ };
class StubGpioController : public IGpioController { /* ... */ };

// Context uses strategy
class WallboxController {
    std::unique_ptr<IGpioController> m_gpio; // Strategy
};
```

---

### 2. Observer Pattern

**"Define a one-to-many dependency between objects"**

✅ **Used for:**

- **State change notifications**: `ChargingStateMachine` notifies `WallboxController`

```cpp
// Observer callback type
using StateChangeCallback = std::function<void(
    ChargingState oldState,
    ChargingState newState,
    const std::string &reason
)>;

// Subject (Observable)
class ChargingStateMachine {
    void addStateChangeListener(StateChangeCallback callback);
private:
    std::vector<StateChangeCallback> m_listeners;
    void notifyStateChange(/* ... */);
};

// Observer registration
m_stateMachine->addStateChangeListener(
    [this](ChargingState oldState, ChargingState newState, const std::string &reason) {
        onStateChange(oldState, newState, reason);
    }
);
```

---

### 3. State Pattern

**"Allow an object to alter its behavior when its internal state changes"**

✅ **Used for:**

- **Charging state management**: `ChargingStateMachine`

```cpp
enum class ChargingState {
    IDLE,      // Ready, no vehicle
    PREPARING, // Vehicle detected
    CHARGING,  // Active charging
    PAUSED,    // Charging paused
    FINISHING, // Completing
    ERROR,     // Error state
    DISABLED   // Maintenance mode
};

class ChargingStateMachine {
    ChargingState m_currentState;

    bool transitionTo(ChargingState newState, const std::string &reason);
    bool canTransitionTo(ChargingState newState) const;
    bool isValidTransition(ChargingState from, ChargingState to) const;
};
```

**State transitions enforced:**

- `IDLE` → `PREPARING` → `CHARGING`
- `CHARGING` → `PAUSED` → `CHARGING`
- Any state → `ERROR` or `DISABLED`

---

### 4. Facade Pattern

**"Provide a unified interface to a set of interfaces in a subsystem"**

✅ **Used for:**

- **WallboxController**: Simplifies complex subsystem interactions

```cpp
class WallboxController {
public:
    // Simple facade methods hide complexity
    bool startCharging();    // Coordinates GPIO, state machine, network
    bool stopCharging();
    bool pauseCharging();

private:
    // Complex subsystems
    std::unique_ptr<IGpioController> m_gpio;
    std::unique_ptr<INetworkCommunicator> m_network;
    std::unique_ptr<ChargingStateMachine> m_stateMachine;

    // Internal coordination
    void setupGpio();
    void updateLeds();
    void sendStatusToSimulator();
};
```

---

### 5. Factory Pattern

**"Create objects without specifying the exact class"**

✅ **Used for:**

- **GPIO factory**: `GpioFactory` creates appropriate GPIO controller

```cpp
class GpioFactory {
public:
    static std::unique_ptr<IGpioController> create(bool useStub = false) {
        if (useStub) {
            return std::make_unique<StubGpioController>();
        }
        return std::make_unique<BananaPiGpioController>();
    }
};

// Usage
auto gpio = GpioFactory::create(isTestMode);
```

---

### 6. Dependency Injection Pattern

**"Inject dependencies rather than creating them"**

✅ **Used throughout:**

```cpp
// Constructor injection (preferred)
WallboxController(
    std::unique_ptr<IGpioController> gpio,
    std::unique_ptr<INetworkCommunicator> network
);

// Benefits:
// - Testability: Inject mocks/stubs
// - Flexibility: Swap implementations
// - Loose coupling: No hard dependencies
```

---

### 7. Command Pattern

**"Encapsulate a request as an object"**

✅ **Used for:**

- **State transitions**: Each transition is a command with reason

```cpp
// Commands encapsulate state change requests
bool startCharging(const std::string &reason = "User requested");
bool stopCharging(const std::string &reason = "User requested");
bool pauseCharging(const std::string &reason = "User requested");

// Each command includes:
// - Action (state transition)
// - Reason (for logging/debugging)
// - Validation (canTransitionTo)
// - Execution (transitionTo)
// - Notification (observers)
```

---

## Architecture Benefits

### 1. Testability

```cpp
// Easy to test with stubs
auto stubGpio = std::make_unique<StubGpioController>();
auto stubNetwork = std::make_unique<MockNetworkCommunicator>();
auto controller = std::make_unique<WallboxController>(
    std::move(stubGpio),
    std::move(stubNetwork)
);

// Test without hardware!
controller->startCharging();
ASSERT_EQ(controller->getCurrentState(), ChargingState::CHARGING);
```

### 2. Extensibility

```cpp
// Add new hardware without changing existing code
class RaspberryPiGpioController : public IGpioController {
    // Implement interface for Raspberry Pi
};

// Just swap the implementation!
auto gpio = std::make_unique<RaspberryPiGpioController>();
```

### 3. Maintainability

- **Clear separation of concerns**
- **Each class has single responsibility**
- **Dependencies explicit via interfaces**
- **Easy to locate and fix bugs**

### 4. Flexibility

- **Runtime behavior changes**
- **Configuration-driven**
- **Multiple deployment targets**

---

## Code Quality Metrics

| Metric                    | Status                                     |
| ------------------------- | ------------------------------------------ |
| **Single Responsibility** | ✅ Each class has one reason to change     |
| **Open/Closed**           | ✅ Extensible via interfaces               |
| **Liskov Substitution**   | ✅ All implementations are interchangeable |
| **Interface Segregation** | ✅ Focused, minimal interfaces             |
| **Dependency Inversion**  | ✅ Depends on abstractions                 |
| **Testability**           | ✅ Easy to mock/stub                       |
| **Loose Coupling**        | ✅ Via interfaces and DI                   |
| **High Cohesion**         | ✅ Related functionality grouped           |

---

## Design Pattern Summary

| Pattern                  | Location          | Purpose                    |
| ------------------------ | ----------------- | -------------------------- |
| **Strategy**             | GPIO, Network     | Interchangeable algorithms |
| **Observer**             | State Machine     | State change notifications |
| **State**                | Charging States   | Manage state transitions   |
| **Facade**               | WallboxController | Simplify subsystem access  |
| **Factory**              | GpioFactory       | Object creation            |
| **Dependency Injection** | Throughout        | Loose coupling             |
| **Command**              | State transitions | Encapsulate requests       |

---

## Future Improvements

### Potential Additions:

1. **Chain of Responsibility**: For error handling pipeline
2. **Template Method**: For charging algorithms
3. **Adapter**: For third-party hardware APIs
4. **Decorator**: For adding features to GPIO/Network

### Already Well-Implemented:

- ✅ SOLID principles throughout
- ✅ Multiple design patterns
- ✅ Clear architecture
- ✅ Good separation of concerns
- ✅ High testability

---

## Conclusion

The Wallbox Controller codebase demonstrates professional software engineering practices:

- **SOLID principles** are consistently applied
- **Multiple design patterns** work together harmoniously
- **Clean architecture** with clear boundaries
- **High testability** via dependency injection
- **Easy to extend** without modification

This design supports long-term maintainability and evolution of the system.
