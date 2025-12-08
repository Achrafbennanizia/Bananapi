# Development Guide

Guide for developers contributing to the ISO 15118 Wallbox Control System.

## Table of Contents

- [Development Setup](#development-setup)
- [Code Structure](#code-structure)
- [Coding Standards](#coding-standards)
- [Building from Source](#building-from-source)
- [Testing](#testing)
- [Debugging](#debugging)
- [Contributing](#contributing)
- [Release Process](#release-process)

## Development Setup

### Required Tools

```bash
# Ubuntu/Debian
sudo apt-get install -y \
    git \
    g++ \
    cmake \
    make \
    gdb \
    valgrind \
    clang-format \
    clang-tidy

# macOS
brew install \
    git \
    cmake \
    llvm \
    clang-format

# Install development headers
sudo apt-get install -y \
    build-essential \
    linux-headers-$(uname -r)
```

### IDE Setup

#### VS Code

```bash
# Install extensions
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools
code --install-extension twxs.cmake

# Open project
code /path/to/Bananapi
```

`.vscode/settings.json`:

```json
{
  "C_Cpp.default.cppStandard": "c++17",
  "C_Cpp.default.compilerPath": "/usr/bin/g++",
  "cmake.configureOnOpen": true,
  "files.associations": {
    "*.h": "cpp",
    "*.cpp": "cpp"
  }
}
```

#### CLion

1. Open project from CMakeLists.txt
2. Configure toolchain (Settings → Build → Toolchains)
3. Set C++ standard to C++17

### Git Configuration

```bash
# Clone with submodules
git clone --recursive https://github.com/Achrafbennanizia/Bananapi.git

# Configure user
git config user.name "Your Name"
git config user.email "your.email@example.com"

# Create development branch
git checkout -b feature/your-feature
```

## Code Structure

### Directory Layout

```
Bananapi/
├── LibPubWallbox/           # Protocol library
│   ├── *.h                  # Header files
│   ├── *.cpp                # Implementation files
│   └── Makefile             # Build system
│
├── WallboxCtrl/             # Main application
│   ├── src/                 # Source code
│   │   ├── main.cpp         # Wallbox controller
│   │   ├── simulator.cpp    # ISO stack simulator
│   │   └── *.cpp            # Helper implementations
│   │
│   ├── include/             # Public headers
│   ├── build/               # Build output
│   └── CMakeLists.txt       # Build configuration
│
├── docs/                    # Documentation
└── tests/                   # Test files
```

### Module Overview

#### Wallbox Controller (`main.cpp`)

**Responsibilities:**

- UDP communication management
- GPIO control
- Watchdog monitoring
- Terminal interface
- Safety logic

**Key Functions:**

```cpp
void setElectricity(bool on);           // Control relay
void recv_state(int sock);              // Receive ISO state
void send_cmd(int sock, sockaddr_in&);  // Send commands
void process_command(const string&);    // Terminal commands
```

#### ISO Simulator (`simulator.cpp`)

**Responsibilities:**

- ISO state machine
- Charging state management
- Hardware command generation
- Terminal interface

**Key Functions:**

```cpp
void recv_cmd(int sock);                // Receive wallbox commands
void send_state(int sock, sockaddr_in&);// Send ISO state
void process_command(const string&);    // Terminal commands
```

#### Protocol Library (`IsoStackCtrlProtocol.cpp`)

**Responsibilities:**

- Message structure definitions
- Endianness conversion
- Protocol helpers

## Coding Standards

### Style Guide

**General Rules:**

- **Indentation:** 4 spaces (no tabs)
- **Line length:** 120 characters maximum
- **Braces:** K&R style (opening brace on same line)
- **Comments:** `//` for single-line, `/* */` for multi-line

**Naming Conventions:**

```cpp
// Classes and structs
class WallboxController { };
struct StateInfo { };

// Functions
void sendCommand();
bool isEnabled();

// Variables
int currentDemand;
bool g_isRunning;              // Global prefix: g_
const int MAX_CURRENT = 6500;  // Constants: UPPER_CASE

// Enums
enum class ChargingState {
    idle,
    charging
};
```

### Code Example

```cpp
/**
 * @brief Send ISO stack command to simulator
 * @param sock UDP socket file descriptor
 * @param dest Destination address
 * @param enable Charging enable flag
 * @param current Current demand in 0.1A
 * @return true if sent successfully, false otherwise
 */
bool sendIsoCommand(int sock, const sockaddr_in& dest,
                   bool enable, uint16_t current)
{
    // Validate parameters
    if (sock < 0) {
        std::cerr << "Invalid socket\n";
        return false;
    }

    if (current > MAX_CURRENT) {
        std::cerr << "Current out of range\n";
        return false;
    }

    // Prepare command
    stSeIsoStackCmd cmd;
    cmd.isoStackCmd.clear();
    cmd.isoStackCmd.msgVersion = 0;
    cmd.isoStackCmd.msgType = enIsoStackMsgType::SeCtrlCmd;
    cmd.isoStackCmd.enable = enable ? 1 : 0;
    cmd.isoStackCmd.currentDemand = current;

    // Send message
    ssize_t n = sendto(sock, &cmd, sizeof(cmd), 0,
                      (const sockaddr*)&dest, sizeof(dest));

    if (n < 0) {
        perror("sendto");
        return false;
    }

    return true;
}
```

### Format Code

```bash
# Format single file
clang-format -i main.cpp

# Format all C++ files
find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Check format without modifying
clang-format --dry-run --Werror main.cpp
```

`.clang-format`:

```yaml
BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 120
BreakBeforeBraces: Linux
AllowShortFunctionsOnASingleLine: Empty
```

## Building from Source

### Debug Build

```bash
cd WallboxCtrl/src

# Compile with debug symbols
g++ -std=c++17 -g -Wall -Wextra \
    main.cpp IsoStackCtrlProtocol_impl.cpp \
    -o wallbox_ctrl_debug

g++ -std=c++17 -g -Wall -Wextra \
    simulator.cpp IsoStackCtrlProtocol_impl.cpp \
    -o simulator_debug
```

### Release Build

```bash
# Optimized build
g++ -std=c++17 -O3 -DNDEBUG -Wall \
    main.cpp IsoStackCtrlProtocol_impl.cpp \
    -o wallbox_ctrl

g++ -std=c++17 -O3 -DNDEBUG -Wall \
    simulator.cpp IsoStackCtrlProtocol_impl.cpp \
    -o simulator
```

### CMake Build

```bash
cd WallboxCtrl
mkdir -p build && cd build

# Debug configuration
cmake -DCMAKE_BUILD_TYPE=Debug ..
make VERBOSE=1

# Release configuration
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### Build with Sanitizers

```bash
# Address sanitizer (memory errors)
g++ -std=c++17 -g -fsanitize=address \
    main.cpp IsoStackCtrlProtocol_impl.cpp \
    -o wallbox_ctrl_asan

# Thread sanitizer (race conditions)
g++ -std=c++17 -g -fsanitize=thread \
    main.cpp IsoStackCtrlProtocol_impl.cpp \
    -o wallbox_ctrl_tsan

# Undefined behavior sanitizer
g++ -std=c++17 -g -fsanitize=undefined \
    main.cpp IsoStackCtrlProtocol_impl.cpp \
    -o wallbox_ctrl_ubsan
```

## Testing

### Unit Tests

Create `tests/test_protocol.cpp`:

```cpp
#include "../LibPubWallbox/IsoStackCtrlProtocol.h"
#include <cassert>
#include <iostream>

void test_clear_functions() {
    Iso15118::stIsoStackCmd cmd;
    cmd.clear();
    assert(cmd.msgVersion == 0);
    assert(cmd.enable == 0);
    std::cout << "✓ clear_functions test passed\n";
}

void test_message_size() {
    assert(sizeof(Iso15118::stSeIsoStackCmd) == 24);
    assert(sizeof(Iso15118::stSeIsoStackState) == 40);
    std::cout << "✓ message_size test passed\n";
}

int main() {
    test_clear_functions();
    test_message_size();
    std::cout << "\n All tests passed!\n";
    return 0;
}
```

Build and run:

```bash
g++ -std=c++17 tests/test_protocol.cpp \
    WallboxCtrl/src/IsoStackCtrlProtocol_impl.cpp \
    -o test_protocol
./test_protocol
```

### Integration Tests

```bash
# Start both programs in background
./wallbox_ctrl > wallbox.log 2>&1 &
WB_PID=$!

./simulator > simulator.log 2>&1 &
SIM_PID=$!

# Give them time to start
sleep 2

# Send test commands
echo "enable" | nc -u localhost 50010
sleep 1
echo "status" | nc -u localhost 50010

# Check logs
grep -q "enable=true" simulator.log && echo "✓ Communication OK"

# Cleanup
kill $WB_PID $SIM_PID
```

### Performance Testing

```bash
# Monitor resource usage
top -p $(pgrep wallbox_ctrl)

# Profile with perf
perf record -g ./wallbox_ctrl
perf report

# Memory leak detection
valgrind --leak-check=full ./wallbox_ctrl
```

## Debugging

### GDB Debugging

```bash
# Compile with debug symbols
g++ -std=c++17 -g main.cpp IsoStackCtrlProtocol_impl.cpp -o wallbox_ctrl

# Start debugger
gdb ./wallbox_ctrl

# GDB commands:
(gdb) break main            # Set breakpoint
(gdb) run                   # Start program
(gdb) next                  # Step over
(gdb) step                  # Step into
(gdb) print variable        # Print variable value
(gdb) backtrace            # Show call stack
(gdb) continue             # Continue execution
```

### Network Debugging

```bash
# Monitor UDP traffic
sudo tcpdump -i lo -n port 50010 or port 50011

# Watch packets with content
sudo tcpdump -i lo -X -n port 50010

# Use netcat to test
echo "test" | nc -u localhost 50010
```

### Add Debug Logging

```cpp
#ifdef DEBUG
#define DEBUG_LOG(msg) std::cout << "[DEBUG] " << msg << "\n"
#else
#define DEBUG_LOG(msg)
#endif

// Usage:
DEBUG_LOG("Received state: " << state.isoStackState.state);
```

Compile with debug:

```bash
g++ -std=c++17 -DDEBUG main.cpp ... -o wallbox_ctrl
```

## Contributing

### Workflow

1. **Fork the repository**

   ```bash
   # On GitHub: Fork → Your Account
   git clone https://github.com/YOUR_USERNAME/Bananapi.git
   cd Bananapi
   git remote add upstream https://github.com/Achrafbennanizia/Bananapi.git
   ```

2. **Create feature branch**

   ```bash
   git checkout -b feature/my-new-feature
   ```

3. **Make changes**

   ```bash
   # Edit files
   # Test changes
   # Format code
   clang-format -i *.cpp *.h
   ```

4. **Commit changes**

   ```bash
   git add .
   git commit -m "Add feature: description"
   ```

5. **Push and create PR**
   ```bash
   git push origin feature/my-new-feature
   # Create Pull Request on GitHub
   ```

### Commit Messages

Format:

```
<type>: <subject>

<body>

<footer>
```

Types:

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation
- `style`: Formatting
- `refactor`: Code restructuring
- `test`: Tests
- `chore`: Maintenance

Example:

```
feat: Add support for three-phase charging

- Implement ac3 phase detection
- Calculate power for three-phase supply
- Update documentation

Closes #123
```

### Pull Request Checklist

- [ ] Code follows style guidelines
- [ ] All tests pass
- [ ] Documentation updated
- [ ] Commit messages are clear
- [ ] No merge conflicts
- [ ] Changes are backwards compatible

## Release Process

### Version Numbering

Format: `MAJOR.MINOR.PATCH`

- **MAJOR**: Breaking changes
- **MINOR**: New features (backwards compatible)
- **PATCH**: Bug fixes

### Creating a Release

1. **Update version**

   ```cpp
   // version.h
   #define VERSION_MAJOR 1
   #define VERSION_MINOR 2
   #define VERSION_PATCH 0
   ```

2. **Update CHANGELOG.md**

   ```markdown
   ## [1.2.0] - 2025-12-08

   ### Added

   - Three-phase charging support
   - New debug mode

   ### Fixed

   - Watchdog timeout issue
   ```

3. **Tag release**

   ```bash
   git tag -a v1.2.0 -m "Release version 1.2.0"
   git push origin v1.2.0
   ```

4. **Build release binaries**

   ```bash
   ./build_release.sh
   ```

5. **Create GitHub release**
   - Go to Releases → New Release
   - Select tag v1.2.0
   - Upload binaries
   - Add release notes

## Additional Resources

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [ISO 15118 Standard](https://www.iso.org/standard/55366.html)
- [CMake Documentation](https://cmake.org/documentation/)
- [GDB Tutorial](https://www.gnu.org/software/gdb/documentation/)

## Getting Help

- GitHub Issues: [Report bugs](https://github.com/Achrafbennanizia/Bananapi/issues)
- Discussions: [Ask questions](https://github.com/Achrafbennanizia/Bananapi/discussions)
- Email: achraf.bennani@example.com
