# Changelog

All notable changes to the ISO 15118 Wallbox Control System will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned

- Web-based monitoring dashboard
- MQTT integration
- REST API for remote control
- Database logging (SQLite/PostgreSQL)
- Mobile app (Android/iOS)
- Load balancing for multiple charging points

## [1.0.0] - 2025-12-08

### Added

- Complete documentation suite
  - Comprehensive README with architecture diagrams
  - Installation guide for multiple platforms
  - API reference documentation
  - Development guide for contributors
  - This changelog
- Support for macOS, Linux, Raspberry Pi, and Banana Pi
- Docker containerization support

### Changed

- Improved error messages in simulator
- Enhanced status display formatting

### Fixed

- Scope issue with `g_lastRelayState` variable in simulator
- Redundant warning messages in state transitions

## [0.9.0] - 2025-11-25

### Added

- Interactive terminal interface for wallbox controller
  - Commands: enable, disable, status, help, quit
- Interactive terminal interface for ISO stack simulator
  - Commands: on, off, idle, ready, charge, stop, status, help, quit
- Optimized logging (only shows state changes)
- Non-blocking I/O for concurrent terminal and network input
- Watchdog timeout safety feature (2 seconds)
- Main contactor state verification before state transitions

### Changed

- Refactored command processing with clearer error messages
- Improved user feedback for failed operations

## [0.8.0] - 2025-11-24

### Added

- UDP bidirectional communication between wallbox and simulator
- ISO 15118 Stack Simulator implementation
  - State machine for charging states (idle, ready, charging, stop, etc.)
  - Hardware command generation (main contactor, voltage, current)
- Wallbox controller with GPIO support
  - HAL abstraction layer for Raspberry Pi/Banana Pi
  - Relay control via GPIO pin 5
  - UDP message handling
- Protocol implementation (IsoStackCtrlProtocol)
  - Message structures: stSeIsoStackCmd, stSeIsoStackState
  - Endianness conversion
  - Helper functions

### Technical Details

- C++17 standard
- Update rate: 10 Hz (100ms intervals)
- Ports: 50010 (wallbox), 50011 (simulator)
- Message sizes: 24 bytes (cmd), 40 bytes (state)

## [0.7.0] - 2025-11-23

### Added

- Alternative implementation with WallboxCore class
- PV power integration support
- MCU signal handling
- CMake build system
- Build automation scripts

## [0.6.0] - 2025-11-22

### Added

- LibPubWallbox integration
  - IsoStackCtrlProtocol library compilation
  - Removed LibCpp dependency
  - Local BigEndian implementation

### Changed

- Fixed include paths for cross-platform compatibility
- Modified struct initialization for C++ standard compliance

## [0.5.0] - 2025-11-20

### Added

- Initial project structure
- Basic wallbox control logic
- ISO 15118 protocol definitions
- Docker environment setup

### Technical Stack

- C++17
- POSIX sockets (UDP)
- CMake and Make build systems
- Docker for containerization

## Version History Summary

| Version | Date       | Description                    |
| ------- | ---------- | ------------------------------ |
| 1.0.0   | 2025-12-08 | Complete documentation release |
| 0.9.0   | 2025-11-25 | Interactive terminal interface |
| 0.8.0   | 2025-11-24 | UDP communication & simulator  |
| 0.7.0   | 2025-11-23 | WallboxCore implementation     |
| 0.6.0   | 2025-11-22 | LibPubWallbox integration      |
| 0.5.0   | 2025-11-20 | Initial implementation         |

## Migration Guides

### Upgrading to 1.0.0

No breaking changes. All features from 0.9.0 remain compatible.

**New features:**

- Comprehensive documentation in `/docs` directory
- Installation scripts improved
- Development guidelines established

**Action required:**

- None. Simply pull latest changes.

### Upgrading to 0.9.0

**Breaking changes:**

- None

**New features:**

- Interactive terminal commands added
- Optimized logging (less verbose)

**Migration steps:**

1. Rebuild programs: `g++ -std=c++17 -O2 main.cpp ... -o wallbox_ctrl`
2. Run and test new terminal commands

### Upgrading to 0.8.0

**Breaking changes:**

- Changed communication from local function calls to UDP
- Requires both wallbox_ctrl and simulator to run

**Migration steps:**

1. Compile both programs separately
2. Run in separate terminals
3. Configure firewall to allow UDP ports 50010-50011

## Notes

- All releases are available on [GitHub Releases](https://github.com/Achrafbennanizia/Bananapi/releases)
- Binary packages provided for Linux (x86_64, ARM), macOS (ARM64, x86_64)
- Docker images available on Docker Hub

## Contributing

See [DEVELOPMENT.md](DEVELOPMENT.md) for contribution guidelines.

Report bugs and request features on [GitHub Issues](https://github.com/Achrafbennanizia/Bananapi/issues).

---

**Legend:**

- `Added` - New features
- `Changed` - Changes in existing functionality
- `Deprecated` - Soon-to-be removed features
- `Removed` - Removed features
- `Fixed` - Bug fixes
- `Security` - Security patches
