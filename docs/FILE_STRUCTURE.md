# Project File Structure v4.1

**Industry Standard Organization - December 2024**

## Directory Structure

```
PJMT/
├── build/                # Build output (gitignored)
│   ├── bin/              # Executables
│   └── lib/              # Libraries
├── cmake/                # CMake modules
├── config/               # Configuration files
│   ├── production.json
│   ├── development.json
│   └── test.json
├── docs/                 # Documentation
│   ├── api/              # API reference
│   ├── architecture/     # Architecture docs
│   ├── guides/           # User guides
│   └── doxygen/          # Generated API docs
├── examples/             # Example code
├── external/             # Third-party dependencies
│   └── LibPubWallbox/    # ISO 15118 protocol
├── include/              # Public headers
│   └── wallbox/          # Namespace headers
├── scripts/              # Automation scripts
│   ├── build/            # Build scripts
│   ├── deploy/           # Deployment
│   └── test/             # Test scripts
├── src/                  # Source code
│   ├── core/             # Core logic
│   ├── gpio/             # GPIO implementations
│   ├── network/          # UDP communication
│   ├── signal/           # CP Signal system
│   ├── api/              # HTTP API
│   └── simulator/        # ISO 15118 simulator
├── tests/                # Test code
│   ├── integration/      # Integration tests
│   ├── unit/             # Unit tests
│   └── fixtures/         # Test data
├── tools/                # Development tools
├── web/                  # Web interfaces
│   └── react-app/        # React dashboard
├── CMakeLists.txt        # Root CMake
├── Doxyfile              # Doxygen config
├── README.md             # Main readme
└── VERSION               # Version: 4.1.0
```

## Build Output

```bash
build/
├── bin/
│   ├── simulator              # ISO 15118 simulator
│   ├── wallbox_control_v1     # Basic version
│   ├── wallbox_control_v2     # State machine version
│   └── wallbox_control_v4     # Latest with CP Signal
└── lib/
    ├── libwallbox_core.a      # Core functionality
    └── libwallbox_api.a       # HTTP API
```

## Key Changes from v4.0

1. **Unified Source Structure** - All source in `src/` organized by function
2. **Standard Include Path** - Headers in `include/wallbox/`
3. **Organized Scripts** - All scripts in `scripts/{build,deploy,test}/`
4. **Consolidated Docs** - All documentation in `docs/`
5. **Environment Configs** - Separate configs for production/development/test
6. **Clean Build System** - Single root CMakeLists.txt

## Building

```bash
# Quick build
mkdir build && cd build
cmake -DBUILD_MODE=production ..
make -j$(nproc)

# Interactive install
./scripts/deploy/install.sh --interactive
```

## Migration from Old Structure

| Old                    | New                                |
| ---------------------- | ---------------------------------- |
| `WallboxCtrl/src/`     | `src/core/`, `src/gpio/`, etc.     |
| `WallboxCtrl/include/` | `include/wallbox/`                 |
| Root `*.sh`            | `scripts/deploy/`, `scripts/test/` |
| Root `*.md`            | `docs/`                            |
| `LibPubWallbox/`       | `external/LibPubWallbox/`          |
| `web/react-app/`   | `web/react-app/`                   |

## See Also

- [Quick Start](guides/QUICK_START.md)
- [Installation Guide](guides/INSTALLATION_GUIDE.md)
- [API Reference](doxygen/html/index.html)
