# Architecture Documentation

This directory contains comprehensive documentation about the system's architecture, design principles, and structural patterns.

## 📐 Architecture Overview

The Wallbox Control System is built on **Clean Architecture** principles with **SOLID** design patterns, ensuring maintainability, testability, and scalability.

### Core Principles

1. **Clean Architecture** - Separation of concerns with clear dependency rules
2. **SOLID Principles** - Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion
3. **Design Patterns** - Strategy, Factory, Observer, Command, State, Singleton, Dependency Injection
4. **Interface-Based Design** - Abstractions over implementations
5. **Dependency Injection** - Constructor-based injection for testability

## 📚 Documentation Files

### Main Architecture Documents

| Document                                             | Description                        | Audience                |
| ---------------------------------------------------- | ---------------------------------- | ----------------------- |
| [ARCHITECTURE_V3.md](ARCHITECTURE_V3.md)             | Current system architecture (v4.1) | All developers          |
| [CLEAN_ARCHITECTURE.md](CLEAN_ARCHITECTURE.md)       | Clean architecture implementation  | Architects, senior devs |
| [CLASS_ARCHITECTURE.md](CLASS_ARCHITECTURE.md)       | Detailed class structure           | All developers          |
| [ARCHITECTURE_VISUAL.md](ARCHITECTURE_VISUAL.md)     | Visual diagrams and flowcharts     | All audiences           |
| [SOLID_DESIGN_PATTERNS.md](SOLID_DESIGN_PATTERNS.md) | Design patterns used               | Developers              |

## 🏗️ System Architecture

### Layer Structure

```
┌─────────────────────────────────────┐
│        Presentation Layer           │
│     (React Dashboard, CLI)          │
└────────────────┬────────────────────┘
                 │
┌────────────────▼────────────────────┐
│        Application Layer            │
│    (Controllers, Use Cases)         │
└────────────────┬────────────────────┘
                 │
┌────────────────▼────────────────────┐
│         Domain Layer                │
│  (Business Logic, State Machines)   │
└────────────────┬────────────────────┘
                 │
┌────────────────▼────────────────────┐
│     Infrastructure Layer            │
│  (GPIO, Network, Persistence)       │
└─────────────────────────────────────┘
```

### Key Components

- **Wallbox Controller** - Main control logic
- **Charging State Machine** - ISO 15118 state management
- **CP Signal Reader** - IEC 61851-1 signal processing
- **GPIO Controller** - Hardware abstraction
- **Network Communicator** - UDP/HTTP communication
- **Configuration Manager** - Settings management

## 🎯 SOLID Principles in Practice

### Single Responsibility Principle (SRP)

Each class has one reason to change:

- `ChargingStateMachine` - Manages only charging states
- `GpioController` - Handles only GPIO operations
- `UdpCommunicator` - Manages only network communication

### Open/Closed Principle (OCP)

Open for extension, closed for modification:

- `IGpioController` interface allows new GPIO implementations without changing existing code
- Strategy pattern for platform-specific implementations

### Liskov Substitution Principle (LSP)

Derived classes are substitutable:

- `BananaPiGpioController` and `StubGpioController` both implement `IGpioController`
- Can swap implementations without breaking code

### Interface Segregation Principle (ISP)

Clients depend only on methods they use:

- `ICpSignalReader` - Only signal reading methods
- `IGpioController` - Only GPIO methods
- `INetworkCommunicator` - Only network methods

### Dependency Inversion Principle (DIP)

Depend on abstractions, not concretions:

- All dependencies injected through constructors
- Controllers depend on interfaces, not implementations

## 🔧 Design Patterns

### Creational Patterns

**Factory Pattern**

```cpp
class GpioFactory
{
public:
    static std::unique_ptr<IGpioController> createController(Platform platform);
};
```

**Singleton Pattern**

```cpp
class Configuration
{
public:
    static Configuration& getInstance();
private:
    Configuration() = default;
};
```

### Structural Patterns

**Strategy Pattern**

```cpp
class WallboxController
{
private:
    std::unique_ptr<IGpioController> m_gpio;  // Strategy
    std::unique_ptr<INetworkCommunicator> m_network;  // Strategy
};
```

### Behavioral Patterns

**State Pattern**

```cpp
class ChargingStateMachine
{
private:
    State m_currentState;
    void transitionTo(State newState);
};
```

**Observer Pattern**

```cpp
class Watchdog
{
public:
    void attach(std::shared_ptr<IObserver> observer);
    void notify();
};
```

**Command Pattern**

```cpp
class Command
{
public:
    virtual void execute() = 0;
};

class StartChargingCommand : public Command
{
    void execute() override { /* implementation */ }
};
```

## 📊 Architecture Layers

### 1. Presentation Layer

**Responsibilities:**

- User interface (React dashboard, CLI)
- Input validation
- Response formatting

**Components:**

- React web application
- Command-line interface
- HTTP API endpoints

### 2. Application Layer

**Responsibilities:**

- Use case orchestration
- Application logic coordination
- API controllers

**Components:**

- `ApiController` - HTTP API handling
- `Application` - Main application coordinator
- `HttpApiServer` - HTTP server management

### 3. Domain Layer

**Responsibilities:**

- Business logic
- Domain models
- State management

**Components:**

- `ChargingStateMachine` - ISO 15118 states
- `WallboxController` - Core control logic
- `CpSignalReader` - CP signal processing

### 4. Infrastructure Layer

**Responsibilities:**

- External system integration
- Hardware abstraction
- Persistence

**Components:**

- `BananaPiGpioController` - Hardware GPIO
- `UdpCommunicator` - Network communication
- `Configuration` - Settings persistence

## 🔄 Data Flow

```
User Input → API Controller → Use Case → Domain Logic → Infrastructure → Hardware
            ←            ←          ←            ←               ←
```

### Request Flow Example

1. **HTTP Request** arrives at `HttpApiServer`
2. **API Controller** validates and routes request
3. **Domain Layer** processes business logic
4. **Infrastructure Layer** executes hardware commands
5. **Response** flows back through layers

## 🧪 Testability

### Dependency Injection

All dependencies are injected through constructors:

```cpp
WallboxController(
    std::unique_ptr<IGpioController> gpio,
    std::unique_ptr<INetworkCommunicator> network
);
```

### Mock Implementations

Test doubles for all interfaces:

- `StubGpioController` - GPIO mock
- Mock network communicator
- In-memory configuration

### Test Pyramid

```
    /\
   /UI\
  /────\
 / API  \
/────────\
/ Unit    \
/──────────\
```

- **Unit Tests**: 80%+ coverage
- **Integration Tests**: Major workflows
- **API Tests**: All endpoints
- **UI Tests**: Critical paths

## 📈 Scalability Considerations

### Horizontal Scaling

- Stateless design
- Message-based communication
- Configuration externalization

### Vertical Scaling

- Efficient resource usage
- Minimal memory footprint
- Fast startup time

### Performance

- Non-blocking I/O
- Thread-safe operations
- Optimized data structures

## 🔒 Security Architecture

### Authentication

- SSH key-based deployment
- No hardcoded credentials
- Secure configuration storage

### Authorization

- Role-based access (future)
- API key authentication (future)
- Rate limiting (future)

### Data Protection

- Input validation
- Error handling
- Secure communication

## 🚀 Future Architecture Plans

### v5.0 Roadmap

- [ ] Microservices architecture option
- [ ] Message queue integration (MQTT)
- [ ] Database persistence layer
- [ ] Advanced monitoring and metrics
- [ ] Distributed deployment support
- [ ] Container orchestration (Kubernetes)

### Evolutionary Architecture

- Continuous refactoring
- Technical debt management
- Architecture decision records (ADRs)
- Regular architecture reviews

## 📖 Related Documentation

- [System Overview](../SYSTEM_OVERVIEW.md)
- [File Structure](../FILE_STRUCTURE.md)
- [Development Guide](../guides/DEVELOPMENT.md)
- [API Reference](../api/API_REFERENCE.md)
- [Doxygen API Docs](../doxygen/html/index.html)

## 🤝 Contributing

When proposing architectural changes:

1. Document the rationale (ADR)
2. Discuss in GitHub Discussions
3. Create proof-of-concept
4. Update architecture docs
5. Submit pull request

See [Contributing Guidelines](../../CONTRIBUTING.md) for details.

## 📚 Further Reading

### Design Patterns

- "Design Patterns" - Gang of Four
- "Head First Design Patterns" - Freeman & Robson

### Clean Architecture

- "Clean Architecture" - Robert C. Martin
- "Clean Code" - Robert C. Martin

### SOLID Principles

- "Agile Software Development" - Robert C. Martin
- "SOLID Principles" - Uncle Bob's blog

---

_This architecture documentation is maintained by the core team. Last updated: v4.1.0 (December 2024)_
