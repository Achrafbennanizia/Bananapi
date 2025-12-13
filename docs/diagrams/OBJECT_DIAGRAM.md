# Wallbox System - Object Diagram

**Date:** December 13, 2025  
**Version:** 4.1 (with CP Signal System)  
**Purpose:** Runtime object instances and relationships

---

## UML Object Diagram

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      WALLBOX SYSTEM OBJECT DIAGRAM                       │
│                    Runtime Instance View (Production Mode)               │
└─────────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────────┐
│                            MAIN CONTROLLER                              │
└────────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────┐
│ wallboxCtrl: WallboxController           │
├──────────────────────────────────────────┤
│ m_running = true                         │
│ m_relayEnabled = false                   │
│ m_wallboxEnabled = true                  │
│ m_currentCpState = STATE_B               │
│ m_operatingMode = "hardware"             │
│ m_gpio = &bananaPiGpio                   │
│ m_network = &udpCommunicator             │
│ m_stateMachine = &chargingStateMachine   │
│ m_cpReader = &hardwareCpReader           │
└──────────────────────────────────────────┘
         │
         │ owns (unique_ptr)
         │
    ┌────┴────┬────────────┬─────────────┐
    │         │            │             │
    ▼         ▼            ▼             ▼


┌────────────────────────────────────────────────────────────────────────┐
│                        OWNED SUBSYSTEM OBJECTS                          │
└────────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────┐
│ bananaPiGpio:                 │
│ BananaPiGpioController        │
├───────────────────────────────┤
│ m_initialized = true          │
│ m_exportedPins = {            │
│   2,  // Relay                │
│   3,  // LED Green            │
│   4,  // LED Yellow           │
│   5,  // LED Red              │
│   6,  // Button               │
│   7   // CP Signal Pin        │
│ }                             │
└───────────────────────────────┘

┌───────────────────────────────┐
│ udpCommunicator:              │
│ UdpCommunicator               │
├───────────────────────────────┤
│ m_listenPort = 50010          │
│ m_sendPort = 50011            │
│ m_sendAddress = "127.0.0.1"   │
│ m_socketFd = 5                │
│ m_connected = true            │
│ m_receiving = true            │
│ m_receiveThread = <thread>    │
└───────────────────────────────┘

┌───────────────────────────────┐
│ chargingStateMachine:         │
│ ChargingStateMachine          │
├───────────────────────────────┤
│ m_currentState = CONNECTED    │
│ m_previousState = IDLE        │
│ m_listeners = [               │
│   &wallboxCtrl.onStateChange  │
│ ]                             │
└───────────────────────────────┘

┌───────────────────────────────┐
│ hardwareCpReader:             │
│ HardwareCpSignalReader        │
├───────────────────────────────┤
│ m_cpPin = 7                   │
│ m_currentState = STATE_B      │
│ m_monitoring = true           │
│ m_initialized = true          │
│ m_gpio = &bananaPiGpio        │
│ m_monitorThread = <thread>    │
│ m_callbacks = [               │
│   &wallboxCtrl.onCpStateChange│
│ ]                             │
└───────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                            HTTP API SERVER                              │
└────────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────┐
│ httpServer: HttpApiServer     │
├───────────────────────────────┤
│ m_port = 8080                 │
│ m_running = true              │
│ m_controller = &wallboxCtrl   │
│ m_daemon = <MHD_Daemon*>      │
└───────────────────────────────┘
         │
         │ uses (raw pointer)
         │
         ▼
┌───────────────────────────────┐
│ wallboxCtrl: WallboxController│
└───────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                       SINGLETON CONFIGURATION                           │
└────────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────┐
│ config: Configuration         │
│ [singleton instance]          │
├───────────────────────────────┤
│ m_mode = HARDWARE             │
│ m_udpListenPort = 50010       │
│ m_udpSendPort = 50011         │
│ m_udpSendAddress = "127.0.0.1"│
│ m_apiPort = 8080              │
│ m_relayPin = 2                │
│ m_ledGreenPin = 3             │
│ m_ledYellowPin = 4            │
│ m_ledRedPin = 5               │
│ m_buttonPin = 6               │
│ m_cpPin = 7                   │
│ m_maxCurrentAmps = 32         │
│ m_voltage = 230               │
│ m_configFilePath =            │
│   "/etc/wallbox/config.json"  │
└───────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                         OBJECT INTERACTIONS                             │
└────────────────────────────────────────────────────────────────────────┘

[SCENARIO: Vehicle Connection Event]

1. hardwareCpReader.monitorLoop()
   ├─> Reads GPIO pin 7 via bananaPiGpio
   ├─> Voltage = 9V detected
   ├─> Maps to CpState::STATE_B
   └─> Calls m_callbacks[0](&wallboxCtrl.onCpStateChange)

2. wallboxCtrl.onCpStateChange(STATE_B)
   ├─> Updates m_currentCpState = STATE_B
   ├─> Calls mapCpStateToChargingState()
   └─> Determines: STATE_B → CONNECTED

3. wallboxCtrl.mapCpStateToChargingState()
   └─> Calls chargingStateMachine.transitionTo(CONNECTED)

4. chargingStateMachine.transitionTo(CONNECTED)
   ├─> Validates transition: IDLE → CONNECTED ✅
   ├─> Updates m_currentState = CONNECTED
   ├─> Updates m_previousState = IDLE
   └─> Calls notifyStateChange()

5. chargingStateMachine.notifyStateChange()
   └─> Invokes m_listeners[0](CONNECTED, IDLE)

6. wallboxCtrl.onStateChange(CONNECTED, IDLE)
   ├─> Logs state change
   └─> Updates internal flags


┌────────────────────────────────────────────────────────────────────────┐
│                    MEMORY LAYOUT (PRODUCTION MODE)                      │
└────────────────────────────────────────────────────────────────────────┘

STACK:
┌──────────────────────┐
│ main() function      │
│ ├─ config: Config&   │  ──────────┐
│ └─ ... locals ...    │            │
└──────────────────────┘            │
                                     │ reference to singleton
HEAP:                                │
┌──────────────────────┐            │
│ wallboxCtrl          │◄───────────┘
│ (WallboxController)  │
├──────────────────────┤
│ ├─ m_gpio            │──► ┌──────────────────────┐
│ │  (unique_ptr)      │    │ BananaPiGpioCtrl     │
│ │                    │    │ [heap object]        │
│ │                    │    └──────────────────────┘
│ │                    │
│ ├─ m_network         │──► ┌──────────────────────┐
│ │  (unique_ptr)      │    │ UdpCommunicator      │
│ │                    │    │ [heap object]        │
│ │                    │    │ ├─ m_receiveThread   │
│ │                    │    │ └─ socket resources  │
│ │                    │    └──────────────────────┘
│ │                    │
│ ├─ m_stateMachine    │──► ┌──────────────────────┐
│ │  (unique_ptr)      │    │ ChargingStateMachine │
│ │                    │    │ [heap object]        │
│ │                    │    └──────────────────────┘
│ │                    │
│ └─ m_cpReader        │──► ┌──────────────────────┐
│    (unique_ptr)      │    │ HardwareCpSignalRdr  │
│                      │    │ [heap object]        │
│                      │    │ ├─ m_gpio (shared)   │───┐
│                      │    │ ├─ m_monitorThread   │   │
│                      │    │ └─ m_callbacks       │   │
│                      │    └──────────────────────┘   │
└──────────────────────┘                               │
                                                        │ shared_ptr
┌──────────────────────┐                               │
│ httpServer           │                               │
│ (HttpApiServer)      │                               │
├──────────────────────┤                               │
│ m_controller ────────┼──► wallboxCtrl (raw ptr)     │
│ m_daemon ────────────┼──► libmicrohttpd resources   │
└──────────────────────┘                               │
                                                        │
                                        ┌───────────────┘
                                        ▼
                            ┌──────────────────────┐
                            │ BananaPiGpioCtrl     │
                            │ [shared instance]    │
                            │ ref_count = 2        │
                            └──────────────────────┘

SINGLETON (static storage):
┌──────────────────────┐
│ Configuration        │
│ [singleton]          │
│ Accessed via:        │
│ getInstance()        │
└──────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                     ALTERNATIVE: SIMULATOR MODE                         │
└────────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────┐
│ wallboxCtrl: WallboxController           │
├──────────────────────────────────────────┤
│ m_operatingMode = "simulator"            │
│ m_gpio = &stubGpio                       │◄──┐
│ m_network = &udpCommunicator             │   │
│ m_cpReader = &simulatorCpReader          │◄──┼──┐
└──────────────────────────────────────────┘   │  │
                                                │  │
┌───────────────────────────────┐              │  │
│ stubGpio: StubGpioController  │◄─────────────┘  │
├───────────────────────────────┤                 │
│ m_initialized = true          │                 │
│ m_pinStates = {               │                 │
│   {2, HIGH},  // Relay        │                 │
│   {3, LOW},   // LED Green    │                 │
│   {4, HIGH},  // LED Yellow   │                 │
│   {5, LOW}    // LED Red      │                 │
│ }                             │                 │
└───────────────────────────────┘                 │
                                                   │
┌───────────────────────────────┐                 │
│ simulatorCpReader:            │◄────────────────┘
│ SimulatorCpSignalReader       │
├───────────────────────────────┤
│ m_currentState = STATE_B      │
│ m_initialized = true          │
│ m_network = &udpCommunicator  │──► [shared UDP]
│ m_callbacks = [...]           │
└───────────────────────────────┘


┌────────────────────────────────────────────────────────────────────────┐
│                         OBJECT LIFECYCLE                                │
└────────────────────────────────────────────────────────────────────────┘

[STARTUP SEQUENCE]

1. Configuration::getInstance()
   └─> Singleton created (if not exists)
   └─> Loads config.json

2. CpSignalReaderFactory::create("hardware", gpio, network)
   └─> Creates HardwareCpSignalReader
   └─> Returns unique_ptr<ICpSignalReader>

3. WallboxController constructor
   ├─> Receives unique_ptr<ICpSignalReader>
   ├─> Creates ChargingStateMachine
   ├─> Registers callbacks
   └─> All owned objects initialized

4. wallboxCtrl.initialize()
   ├─> Calls m_gpio->initialize()
   ├─> Calls m_network->connect()
   ├─> Calls m_cpReader->initialize()
   ├─> Calls m_cpReader->startMonitoring()
   └─> Starts monitoring threads

5. HttpApiServer constructor
   └─> Stores raw pointer to wallboxCtrl

6. httpServer.start()
   └─> Creates MHD daemon on port 8080

[SHUTDOWN SEQUENCE]

1. httpServer.stop()
   └─> Destroys MHD daemon

2. wallboxCtrl.shutdown()
   ├─> Calls m_cpReader->stopMonitoring()
   ├─> Calls m_cpReader->shutdown()
   ├─> Calls m_network->disconnect()
   └─> Calls m_gpio->shutdown()

3. WallboxController destructor
   ├─> unique_ptr<ICpSignalReader> destroyed
   │   └─> HardwareCpSignalReader destroyed
   │       └─> Monitoring thread joined
   ├─> unique_ptr<ChargingStateMachine> destroyed
   ├─> unique_ptr<INetworkCommunicator> destroyed
   │   └─> UdpCommunicator destroyed
   │       └─> Receive thread joined
   └─> unique_ptr<IGpioController> destroyed
       └─> BananaPiGpioController destroyed
           └─> GPIO pins unexported

4. Configuration singleton persists (static storage)
   └─> Destroyed at program exit


┌────────────────────────────────────────────────────────────────────────┐
│                          OBJECT STATISTICS                              │
└────────────────────────────────────────────────────────────────────────┘

| Object Instance | Size (bytes) | Lifetime | Owner |
|-----------------|--------------|----------|-------|
| wallboxCtrl | ~256 | Program | Stack/Heap |
| bananaPiGpio | ~128 | Program | WallboxController |
| udpCommunicator | ~96 | Program | WallboxController |
| chargingStateMachine | ~64 | Program | WallboxController |
| hardwareCpReader | ~144 | Program | WallboxController |
| httpServer | ~48 | Program | Stack/Heap |
| config (singleton) | ~192 | Static | None |

**Total Runtime Memory**: ~928 bytes (stack objects)

**Heap Allocations**:
- 5 unique_ptr managed objects
- 2 std::thread objects
- libmicrohttpd daemon resources
- Socket buffers (~8KB per socket)
- JSON parsing buffers (dynamic)

---

**Generated:** December 13, 2025
**Version:** 4.1 with CP Signal System
```
