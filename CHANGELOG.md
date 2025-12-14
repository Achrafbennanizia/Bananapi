# Changelog

All notable changes to the Wallbox Control System project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [4.1.0] - 2024-12-14

### Added

- Industry-standard project file structure
- Unified CMake build system at root level
- Environment-specific configurations (production/development/test)
- Organized script directories (`scripts/build/`, `scripts/deploy/`, `scripts/test/`)
- Consolidated documentation in `docs/` directory
- `include/wallbox/` for public headers
- `external/` for third-party dependencies
- `VERSION` file for version tracking
- Comprehensive `docs/FILE_STRUCTURE.md` documentation
- Build output directory structure (`build/bin/`, `build/lib/`)
- Optional test building with Google Test integration
- Platform-independent include paths

### Changed

- **BREAKING**: Moved all source files from `WallboxCtrl/src/` to organized `src/` subdirectories
  - `src/core/` - Core application logic
  - `src/gpio/` - GPIO implementations
  - `src/network/` - Network communication
  - `src/signal/` - CP Signal processing
  - `src/api/` - HTTP API
  - `src/simulator/` - ISO 15118 simulator
- **BREAKING**: Moved headers from `WallboxCtrl/include/` to `include/wallbox/`
- **BREAKING**: Moved `LibPubWallbox/` to `external/LibPubWallbox/`
- **BREAKING**: Moved `wallbox-react-app/` to `web/react-app/`
- **BREAKING**: Split `config.json` into environment-specific files in `config/`
- **BREAKING**: Updated all include paths to use configured include directories
- Migrated from relative includes (`../../LibPubWallbox/`) to direct includes
- Build system now uses single root CMakeLists.txt
- Updated README.md with new structure and paths
- Improved .gitignore for build artifacts

### Fixed

- Include path issues with LibPubWallbox headers
- CMake configuration for macOS development (using system libcurl)
- Excluded `test_iso.cpp` from simulator build (conflicting main())
- Build process now works cleanly from root directory
- Simplified library dependencies

### Deprecated

- Legacy directory structure (`WallboxCtrl/`, `wallbox-portable-deploy/`, `wallbox-deployment/`)
  - These will be removed in v5.0.0
  - Use unified structure in `src/` and `include/` instead

### Performance

- No performance changes in this release (structural refactoring only)

### Security

- No security changes in this release

### Migration Guide

To migrate from v4.0 to v4.1:

1. **Building**:

   ```bash
   # Old way
   cd WallboxCtrl && mkdir build && cd build && cmake .. && make

   # New way (v4.1)
   mkdir build && cd build && cmake .. && make
   ```

2. **Configuration**:

   ```bash
   # Old: Single config.json in WallboxCtrl/
   # New: Environment-specific configs in config/
   config/production.json    # For production deployment
   config/development.json   # For development
   config/test.json          # For testing
   ```

3. **Scripts**:

   ```bash
   # Old: Scripts scattered across root and subdirectories
   # New: Organized in scripts/
   scripts/deploy/install.sh    # Installation
   scripts/deploy/deploy.sh     # Deployment
   scripts/test/test_wallbox.sh # Testing
   ```

4. **Executables**:
   ```bash
   # Old: WallboxCtrl/build/wallbox_control_v4
   # New: build/bin/wallbox_control_v4
   ```

## [4.0.0] - 2024-12-13

### Added

- Clean terminal output (commands and feedback only)
- Structured logging to `/tmp/wallbox_simulator.log`
- Log categories: [UDP_TX], [UDP_RX], [STATE], [CMD], [WALLBOX], [ERROR]
- Timestamps in log file
- Interactive mode selection in deployment scripts
- `--help`, `--interactive`, `--mode` flags for scripts
- Three build modes: production, development, debug
- Comprehensive help documentation in scripts
- 11 automated integration tests

### Changed

- UDP traffic moved from terminal to log file
- Terminal shows only command feedback and wallbox state changes
- Deployment scripts with interactive menu
- Build scripts with mode validation

### Fixed

- Terminal flooding from UDP messages every 100ms
- Difficult command visibility in verbose output

## [3.0.0] - 2024-11-XX

### Added

- HTTP REST API on port 8080
- CP Signal System (IEC 61851-1)
- Hardware CP Signal Reader
- Simulator CP Signal Reader
- API endpoints for charging control

## [2.0.0] - 2024-10-XX

### Added

- SOLID principles implementation
- Design patterns (Strategy, Observer, Command, State, Factory, Singleton, DI)
- Charging State Machine
- Configuration management with JSON
- Watchdog safety system (2-second timeout)
- Dependency Injection
- Mock implementations for testing

### Changed

- Refactored from monolithic to SOLID architecture
- GPIO abstraction with Strategy pattern
- Network communication with interface segregation

## [1.0.0] - 2024-09-XX

### Added

- Initial release
- Basic ISO 15118 protocol support
- UDP communication (ports 50010/50011)
- Simple relay control
- Command-line interface

---

## Version Numbering

We use [Semantic Versioning](https://semver.org/):

- **MAJOR** version (X.0.0): Incompatible API changes, breaking changes
- **MINOR** version (0.X.0): New features, backwards compatible
- **PATCH** version (0.0.X): Bug fixes, backwards compatible

## Upcoming in v5.0.0

Planned features for next major release:

- [ ] Remove legacy directory structures
- [ ] Complete migration to new CMake system
- [ ] Google Test integration for all tests
- [ ] Enhanced systemd service support
- [ ] Docker container support
- [ ] CI/CD pipelines (GitHub Actions)
- [ ] Automated releases
- [ ] Additional platform support (Raspberry Pi 5)

## Support

- **Documentation**: See `docs/` directory
- **Issues**: Report bugs on GitHub Issues
- **Discussions**: Use GitHub Discussions for questions

## Contributors

Thanks to all contributors who have helped make this project better!

---

_For detailed migration guides and breaking changes, see the version-specific documentation in `docs/guides/`._
