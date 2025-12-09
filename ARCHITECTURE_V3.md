# Wallbox Controller v3.0 - Architecture Improvements

## Overview

Version 3.0 represents a significant architectural improvement over v2.0, introducing industry-standard design patterns and enhanced separation of concerns while maintaining backward compatibility with all existing features.

## 🎯 Key Improvements

### 1. **Separation of Concerns**

- Application logic separated from main() function
- Configuration centralized in dedicated class
- API routing isolated in controller layer

### 2. **Design Patterns Implemented**

#### Singleton Pattern

**Class:** `Configuration`

- Single source of truth for all configuration
- Thread-safe instance management
- Environment variable integration

#### Factory Method Pattern

**Class:** `GpioFactory`

- Centralized GPIO controller creation
- Easy to extend with new GPIO types
- Encapsulates creation logic

#### Controller Pattern (MVC)

**Class:** `ApiController`

- Separates API routing from business logic
- Clear endpoint organization
- Easy to maintain and test

#### Application Controller Pattern

**Class:** `Application`

- Manages entire application lifecycle
- Orchestrates all components
- Clean initialization/shutdown

### 3. **SOLID Principles**

#### Single Responsibility Principle (SRP)

- Each class has ONE reason to change
- `Configuration`: Only manages settings
- `GpioFactory`: Only creates GPIO controllers
- `ApiController`: Only handles API routing
- `Application`: Only manages app lifecycle

#### Open/Closed Principle (OCP)

- Open for extension, closed for modification
- New GPIO types: Add to factory
- New API endpoints: Add to controller
- New config options: Add to Configuration

#### Dependency Inversion Principle (DIP)

- Depend on abstractions, not concretions
- All factories return interfaces
- Controllers use dependency injection

## 📐 Architecture Comparison

### Before (v2.0):

```cpp
// main_v2_with_api.cpp - 288 lines
int main() {
    // Configuration scattered throughout
    const int UDP_LISTEN_PORT = 50010;
    const std::string GPIO_TYPE = "stub";

    // Factory function in main file
    auto gpio = createGpioController(GPIO_TYPE);

    // 100+ lines of setupApiEndpoints() function
    setupApiEndpoints(api, controller);

    // Manual lifecycle management
    // ...
}
```

### After (v3.0):

```cpp
// main_v3.cpp - 69 lines
int main() {
    // Create application
    g_application = std::make_unique<Application>();

    // Initialize
    g_application->initialize();

    // Run
    g_application->run();

    // Shutdown
    g_application->shutdown();
}
```

**Lines of code in main:** 288 → 69 (76% reduction)

## 🏗️ New Architecture Structure

```
┌─────────────────────────────────────────────────┐
│                    main.cpp                     │
│  (Signal handling + Application lifecycle)      │
└────────────────┬────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────┐
│              Application Class                   │
│  • Initialize all components                     │
│  • Manage lifecycle                              │
│  • Coordinate shutdown                           │
└─┬──────────┬──────────────┬─────────────────┬───┘
  │          │              │                 │
  ▼          ▼              ▼                 ▼
┌──────┐  ┌──────┐   ┌─────────────┐   ┌─────────────┐
│Config│  │GPIO  │   │  Wallbox    │   │    API      │
│      │  │Factory│   │ Controller  │   │ Controller  │
└──────┘  └──────┘   └─────────────┘   └─────────────┘
```

## 📁 New File Organization

```
include/
├── Configuration.h        # Singleton - Centralized configuration
├── GpioFactory.h          # Factory - GPIO controller creation
├── ApiController.h        # Controller - API endpoint routing
├── Application.h          # Application Controller - Lifecycle
├── WallboxController.h    # Business logic (unchanged interface)
├── HttpApiServer.h        # Infrastructure (unchanged)
└── [other files...]

src/
├── main_v3.cpp           # Ultra-clean main (69 lines)
├── WallboxController.cpp # Updated to use Configuration::Pins
└── [other files...]
```

## 🎨 Design Patterns Detailed

### 1. Singleton - Configuration

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

### 2. Factory Method - GpioFactory

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

### 3. Controller - ApiController

```cpp
// Clean separation
ApiController apiCtrl(wallboxController);
apiCtrl.setupEndpoints(server);

// Organized by domain
setupHealthEndpoints()
setupStatusEndpoints()
setupChargingEndpoints()
setupWallboxEndpoints()
```

**Benefits:**

- API logic separated from business logic
- Easy to find endpoints
- Testable in isolation

### 4. Application Controller - Application

```cpp
// Manages entire lifecycle
Application app;
app.initialize();  // Setup all components
app.run();         // Main loop
app.shutdown();    // Clean cleanup
```

**Benefits:**

- Encapsulates initialization complexity
- Consistent lifecycle management
- Easy to test startup/shutdown

## 🔄 Migration Guide

### For Developers

**Old way (v2.0):**

```cpp
// Scattered configuration
const int API_PORT = 8080;
auto gpio = createGpioController("stub");
setupApiEndpoints(api, controller);
```

**New way (v3.0):**

```cpp
// Centralized configuration
Configuration& config = Configuration::getInstance();
config.loadFromEnvironment();

// Factory creation
auto gpio = GpioFactory::create(config.getGpioType());

// Controller pattern
ApiController apiCtrl(controller);
apiCtrl.setupEndpoints(server);
```

### For Users

**No changes required!** All executables maintain the same interface:

```bash
# Same environment variable
WALLBOX_MODE=dev ./wallbox_control_v3

# Same API endpoints
curl http://localhost:8080/api/status

# Same React app integration
cd wallbox-react-app && npm start
```

## 📊 Metrics

| Metric                       | v2.0      | v3.0        | Improvement               |
| ---------------------------- | --------- | ----------- | ------------------------- |
| main.cpp lines               | 288       | 69          | ↓ 76%                     |
| Classes                      | 7         | 11          | ↑ 57% (better separation) |
| Design patterns              | 3         | 7           | ↑ 133%                    |
| Configuration centralization | Scattered | Centralized | ✅                        |
| Testability                  | Moderate  | High        | ✅                        |
| Maintainability              | Good      | Excellent   | ✅                        |

## 🧪 Testing Benefits

### Before (v2.0):

```cpp
// Hard to test - requires mocking global functions
void setupApiEndpoints(HttpApiServer& api, WallboxController& ctrl);
```

### After (v3.0):

```cpp
// Easy to test - dependency injection
class ApiController {
    ApiController(WallboxController& ctrl);  // Inject mock
    void setupEndpoints(HttpApiServer& server);
};
```

**Unit testing became trivial:**

```cpp
// Mock wallbox controller
MockWallboxController mockCtrl;

// Test API controller in isolation
ApiController apiCtrl(mockCtrl);
MockHttpServer mockServer;
apiCtrl.setupEndpoints(mockServer);

// Verify endpoints registered
ASSERT_TRUE(mockServer.hasRoute("GET", "/api/status"));
```

## 🚀 Future Extensibility

### Adding New GPIO Type

```cpp
// 1. Create new GPIO controller class
class NewGpioController : public IGpioController { /*...*/ };

// 2. Add to factory (ONE LINE)
if (type == "newtype") {
    return std::make_unique<NewGpioController>();
}
```

### Adding New Configuration

```cpp
// 1. Add to Configuration class
int getNewSetting() const { return m_newSetting; }

// 2. Load from environment
const char* env = std::getenv("WALLBOX_NEW_SETTING");
if (env) m_newSetting = std::stoi(env);
```

### Adding New API Endpoint

```cpp
// Add to ApiController::setup*Endpoints()
server.POST("/api/new/endpoint", [this](...) {
    // Handler logic
});
```

## 📝 Documentation

Each new class includes comprehensive documentation:

- **Purpose**: What the class does
- **Pattern**: Which design pattern it implements
- **SOLID**: Which principles it follows
- **Usage**: How to use it
- **Examples**: Code samples

## 🎯 Recommended Usage

### Development

```bash
# Use v3 for new development
WALLBOX_MODE=dev ./wallbox_control_v3
```

### Production

```bash
# v3 supports all v2 features
WALLBOX_MODE=prod ./wallbox_control_v3
```

### Legacy Support

```bash
# v2 still available for compatibility
./wallbox_control_api  # v2.0 with API
./wallbox_control_v2   # v2.0 SOLID
./wallbox_control      # v1.0 legacy
```

## 🏆 Achievements

✅ **Cleaner Code**: 76% reduction in main.cpp  
✅ **Better Structure**: Clear separation of concerns  
✅ **More Patterns**: 7 design patterns total  
✅ **Easier Testing**: Dependency injection throughout  
✅ **Better Docs**: Comprehensive class documentation  
✅ **Zero Breaking Changes**: Full backward compatibility  
✅ **Future Proof**: Easy to extend and maintain

## Summary

Version 3.0 transforms the codebase from "good" to "excellent" by:

1. **Centralizing** configuration management
2. **Separating** concerns into focused classes
3. **Implementing** industry-standard patterns
4. **Simplifying** the main function
5. **Improving** testability
6. **Enhancing** extensibility

All while maintaining 100% feature parity with v2.0!
