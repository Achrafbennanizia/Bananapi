# Wallbox System - Class Diagram

**Date:** December 13, 2025  
**Version:** 4.1 (with CP Signal System)  
**Purpose:** Complete class structure with relationships and interfaces

---

## UML Class Diagram

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         WALLBOX SYSTEM CLASS DIAGRAM                     │
└─────────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────────┐
│                          MAIN APPLICATION LAYER                         │
└────────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────┐
│     «class»                  │
│    WallboxController         │
├──────────────────────────────┤
│ - m_gpio: unique_ptr<IGpio>  │
│ - m_network: unique_ptr<INet>│
│ - m_stateMachine: unique_ptr │
│ - m_cpReader: unique_ptr<ICP>│
│ - m_running: atomic<bool>    │
│ - m_relayEnabled: bool       │
│ - m_wallboxEnabled: bool     │
│ - m_currentCpState: CpState  │
│ - m_operatingMode: string    │
├──────────────────────────────┤
│ + initialize(): bool         │
│ + shutdown(): void           │
│ + run(): void                │
│ + startCharging(): bool      │
│ + stopCharging(): bool       │
│ + pauseCharging(): bool      │
│ + resumeCharging(): bool     │
│ + enableWallbox(): bool      │
│ + disableWallbox(): bool     │
│ + setRelayState(bool): bool  │
│ + getStatusJson(): string    │
│ - onCpStateChange(): void    │
│ - mapCpStateToCharging(): void│
│ - processNetworkMessage(): void│
└──────────────────────────────┘
         │
         │ uses
         │
    ┌────┴────┐
    │         │
    ▼         ▼


┌────────────────────────────────────────────────────────────────────────┐
│                         INTERFACE LAYER (ABSTRACTIONS)                  │
└────────────────────────────────────────────────────────────────────────┘

┌──────────────────────┐    ┌──────────────────────┐    ┌──────────────────────┐
│  «interface»         │    │  «interface»         │    │  «interface»         │
│  IGpioController     │    │INetworkCommunicator  │    │  ICpSignalReader     │
├──────────────────────┤    ├──────────────────────┤    ├──────────────────────┤
│                      │    │                      │    │                      │
├──────────────────────┤    ├──────────────────────┤    ├──────────────────────┤
│ + initialize(): bool │    │ + connect(): bool    │    │ + initialize(): bool │
│ + shutdown(): void   │    │ + disconnect(): void │    │ + shutdown(): void   │
│ + setPinMode(): bool │    │ + send(data): bool   │    │ + readCpState(): Cp  │
│ + digitalWrite(): bool│   │ + startReceiving(): v│    │ + getCpStateString():│
│ + digitalRead(): Pin │    │ + stopReceiving(): v │    │ + onStateChange(): v │
│ + isInitialized(): b │    │ + isConnected(): bool│    │ + startMonitoring(): │
└──────────────────────┘    └──────────────────────┘    │ + stopMonitoring(): v│
         △                           △                    └──────────────────────┘
         │                           │                             △
         │ implements                │                             │
    ┌────┴────┐                 ┌───┴───┐                    ┌────┴────┐
    │         │                 │       │                    │         │
    ▼         ▼                 ▼       │                    ▼         ▼


┌────────────────────────────────────────────────────────────────────────┐
│                    CONCRETE IMPLEMENTATION LAYER                        │
└────────────────────────────────────────────────────────────────────────┘

┌─────────────────────┐  ┌─────────────────────┐  ┌─────────────────────┐
│  «class»            │  │  «class»            │  │  «class»            │
│BananaPiGpioCtrl     │  │ StubGpioCtrl        │  │ UdpCommunicator     │
├─────────────────────┤  ├─────────────────────┤  ├─────────────────────┤
│ - m_initialized: b  │  │ - m_initialized: b  │  │ - m_listenPort: int │
│ - m_exportedPins: v │  │ - m_pinStates: map  │  │ - m_sendPort: int   │
├─────────────────────┤  ├─────────────────────┤  │ - m_socketFd: int   │
│ + initialize(): b   │  │ + initialize(): b   │  │ - m_receiveThread: t│
│ + shutdown(): void  │  │ + shutdown(): void  │  ├─────────────────────┤
│ - exportPin(): b    │  │ + setPinMode(): b   │  │ + connect(): bool   │
│ - setDirection(): b │  │ + digitalWrite(): b │  │ + disconnect(): void│
│ - setValue(): b     │  │ + digitalRead(): P  │  │ + send(data): bool  │
│ - getValue(): int   │  └─────────────────────┘  │ + startReceiving(): │
└─────────────────────┘                           │ - receiveLoop(): v  │
                                                   └─────────────────────┘

┌─────────────────────┐  ┌─────────────────────┐
│  «class»            │  │  «class»            │
│HardwareCpSignalRdr  │  │SimulatorCpSignalRdr │
├─────────────────────┤  ├─────────────────────┤
│ - m_gpio: shared_ptr│  │ - m_network: shared │
│ - m_cpPin: int      │  │ - m_currentState: Cp│
│ - m_currentState: Cp│  │ - m_initialized: b  │
│ - m_monitoring: atom│  │ - m_callbacks: vect │
│ - m_monitorThread: t│  │ - m_stateMutex: mut │
│ - m_callbacks: vect │  ├─────────────────────┤
│ - m_mutex: mutex    │  │ + initialize(): b   │
├─────────────────────┤  │ + shutdown(): void  │
│ + initialize(): b   │  │ + readCpState(): Cp │
│ + shutdown(): void  │  │ + setCpState(Cp): v │
│ + readCpState(): Cp │  │ + onStateChange(): v│
│ + onStateChange(): v│  │ - handleMessage(): v│
│ + startMonitoring():│  │ - parseMessage(): Cp│
│ - monitorLoop(): v  │  │ - notifyStateChange│
│ - readVoltage(): int│  └─────────────────────┘
│ - voltageToState(): │
│ - notifyStateChange │
└─────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                          FACTORY PATTERN LAYER                          │
└────────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────┐
│  «class»                       │
│  CpSignalReaderFactory         │
│  «static»                      │
├────────────────────────────────┤
│                                │
├────────────────────────────────┤
│ + createSimulatorReader(): ptr│
│ + createHardwareReader(): ptr  │
│ + create(mode, ...): ptr       │
└────────────────────────────────┘
         │
         │ creates
         ▼
┌────────────────────────────────┐
│      ICpSignalReader           │
│      (interface)               │
└────────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                         STATE MACHINE LAYER                             │
└────────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────┐
│  «class»                       │
│  ChargingStateMachine          │
├────────────────────────────────┤
│ - m_currentState: ChargingState│
│ - m_previousState: ChargingState│
│ - m_listeners: vector          │
│ - m_mutex: mutex               │
├────────────────────────────────┤
│ + transitionTo(state): bool    │
│ + canTransitionTo(state): bool │
│ + startCharging(): bool        │
│ + stopCharging(): bool         │
│ + pauseCharging(): bool        │
│ + resumeCharging(): bool       │
│ + enterErrorState(reason): bool│
│ + reset(): bool                │
│ + addStateChangeListener(): v  │
│ + getCurrentState(): State     │
│ + isCharging(): bool           │
│ + isIdle(): bool               │
│ + isError(): bool              │
│ - notifyStateChange(): void    │
│ - isValidTransition(): bool    │
└────────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                          CONFIGURATION LAYER                            │
└────────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────┐
│  «class» «singleton»           │
│  Configuration                 │
├────────────────────────────────┤
│ - m_mode: Mode                 │
│ - m_udpListenPort: int         │
│ - m_udpSendPort: int           │
│ - m_udpSendAddress: string     │
│ - m_apiPort: int               │
│ - m_relayPin: int              │
│ - m_ledGreenPin: int           │
│ - m_ledYellowPin: int          │
│ - m_ledRedPin: int             │
│ - m_buttonPin: int             │
│ - m_cpPin: int                 │
│ - m_maxCurrentAmps: int        │
│ - m_voltage: int               │
├────────────────────────────────┤
│ + getInstance(): Configuration&│
│ + getMode(): Mode              │
│ + getUdpListenPort(): int      │
│ + getCpPin(): int              │
│ + loadFromEnvironment(): void  │
│ + loadFromFile(path): bool     │
│ - Configuration()              │
│ - parseJson(content): void     │
└────────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                           ENUMERATION TYPES                             │
└────────────────────────────────────────────────────────────────────────┘

┌────────────────────┐  ┌────────────────────┐  ┌────────────────────┐
│  «enumeration»     │  │  «enumeration»     │  │  «enumeration»     │
│  CpState           │  │  ChargingState     │  │  PinMode           │
├────────────────────┤  ├────────────────────┤  ├────────────────────┤
│ STATE_A = 0        │  │ OFF = 0            │  │ INPUT              │
│ STATE_B = 1        │  │ IDLE = 1           │  │ OUTPUT             │
│ STATE_C = 2        │  │ CONNECTED = 2      │  │ INPUT_PULLUP       │
│ STATE_D = 3        │  │ IDENTIFICATION = 3 │  └────────────────────┘
│ STATE_E = 4        │  │ READY = 4          │
│ STATE_F = 5        │  │ CHARGING = 5       │  ┌────────────────────┐
│ UNKNOWN = 255      │  │ STOP = 6           │  │  «enumeration»     │
└────────────────────┘  │ FINISHED = 7       │  │  PinValue          │
                        │ ERROR = 8          │  ├────────────────────┤
                        └────────────────────┘  │ LOW = 0            │
                                                 │ HIGH = 1           │
                                                 └────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                          HTTP API LAYER                                 │
└────────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────┐
│  «class»                       │
│  HttpApiServer                 │
├────────────────────────────────┤
│ - m_controller: WallboxCtrl*   │
│ - m_daemon: MHD_Daemon*        │
│ - m_port: int                  │
│ - m_running: atomic<bool>      │
├────────────────────────────────┤
│ + start(): bool                │
│ + stop(): void                 │
│ + isRunning(): bool            │
│ - handleRequest(): int         │
│ - handleStatus(): string       │
│ - handleStart(): string        │
│ - handleStop(): string         │
│ - handleInfo(): string         │
│ - sendJsonResponse(): int      │
└────────────────────────────────┘
         │
         │ uses
         ▼
┌────────────────────────────────┐
│    WallboxController           │
└────────────────────────────────┘
```

---

## Key Relationships

### Inheritance (△)

- `BananaPiGpioController` **implements** `IGpioController`
- `StubGpioController` **implements** `IGpioController`
- `UdpCommunicator` **implements** `INetworkCommunicator`
- `HardwareCpSignalReader` **implements** `ICpSignalReader`
- `SimulatorCpSignalReader` **implements** `ICpSignalReader`

### Composition (◆)

- `WallboxController` **owns** `ChargingStateMachine` (unique_ptr)
- `WallboxController` **owns** `IGpioController` (unique_ptr)
- `WallboxController` **owns** `INetworkCommunicator` (unique_ptr)
- `WallboxController` **owns** `ICpSignalReader` (unique_ptr)

### Aggregation (◇)

- `HttpApiServer` **uses** `WallboxController` (raw pointer)
- `HardwareCpSignalReader` **uses** `IGpioController` (shared_ptr)
- `SimulatorCpSignalReader` **uses** `INetworkCommunicator` (shared_ptr)

### Dependency (- - ->)

- `CpSignalReaderFactory` **creates** `ICpSignalReader` implementations
- All classes **depend on** `Configuration` (singleton)

---

## Design Patterns Applied

| Pattern                  | Classes                                                | Purpose                                    |
| ------------------------ | ------------------------------------------------------ | ------------------------------------------ |
| **Strategy**             | ICpSignalReader, IGpioController, INetworkCommunicator | Interchangeable algorithms                 |
| **Factory**              | CpSignalReaderFactory                                  | Object creation abstraction                |
| **Observer**             | ChargingStateMachine callbacks, CP state callbacks     | Event notification                         |
| **Singleton**            | Configuration                                          | Single global config instance              |
| **Facade**               | WallboxController                                      | Simplified interface to complex subsystems |
| **Dependency Injection** | All constructors                                       | Loose coupling, testability                |

---

## SOLID Principles Compliance

### Single Responsibility Principle (SRP) ✅

- Each class has one clear responsibility
- `ICpSignalReader` → only reads CP signals
- `ChargingStateMachine` → only manages states
- `Configuration` → only handles configuration

### Open/Closed Principle (OCP) ✅

- New CP readers can be added without modifying existing code
- New GPIO implementations can be added via interface

### Liskov Substitution Principle (LSP) ✅

- Any `ICpSignalReader` implementation is substitutable
- Any `IGpioController` implementation is substitutable

### Interface Segregation Principle (ISP) ✅

- Interfaces are minimal and focused
- No client forced to depend on unused methods

### Dependency Inversion Principle (DIP) ✅

- High-level `WallboxController` depends on abstractions
- Low-level implementations depend on abstractions

---

## Class Metrics

| Class                   | LOC | Complexity | Dependencies | Responsibility        |
| ----------------------- | --- | ---------- | ------------ | --------------------- |
| WallboxController       | 673 | High       | 5            | System coordination   |
| ChargingStateMachine    | 283 | Medium     | 0            | State management      |
| BananaPiGpioController  | 245 | Medium     | 0            | Hardware GPIO         |
| HardwareCpSignalReader  | 204 | Medium     | 1            | CP hardware reading   |
| SimulatorCpSignalReader | 316 | Low        | 1            | CP simulation         |
| CpSignalReaderFactory   | 115 | Low        | 2            | Object creation       |
| UdpCommunicator         | 198 | Medium     | 0            | Network communication |
| HttpApiServer           | 347 | Medium     | 1            | REST API              |
| Configuration           | 298 | Low        | 0            | Configuration         |

**Total System**: ~9,400 lines of code across 47 files

---

## Thread Safety

| Class                   | Thread-Safe | Mechanism                      |
| ----------------------- | ----------- | ------------------------------ |
| WallboxController       | ✅ Yes      | atomic<bool>, careful ordering |
| ChargingStateMachine    | ✅ Yes      | std::mutex                     |
| HardwareCpSignalReader  | ✅ Yes      | std::mutex, atomic             |
| SimulatorCpSignalReader | ✅ Yes      | std::mutex                     |
| UdpCommunicator         | ✅ Yes      | Dedicated receive thread       |
| HttpApiServer           | ✅ Yes      | libmicrohttpd threading        |
| Configuration           | ⚠️ Const    | Read-only after initialization |

---

**Generated:** December 13, 2025  
**Version:** 4.1 with CP Signal System
