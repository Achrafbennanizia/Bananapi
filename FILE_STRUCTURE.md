# 📂 Project File Structure - Clean Architecture

**Last Updated**: December 10, 2025  
**Version**: 3.0  
**Status**: Production Ready

## 🏗️ Architecture Overview

The project follows a **clean architecture** pattern with clear separation of concerns:

```
PJMT/
│
├── 📚 Documentation Layer (docs/)
├── 🎯 Application Layer (WallboxCtrl/)
├── 🔌 Protocol Layer (LibPubWallbox/)
├── 🌐 Presentation Layer (wallbox-react-app/)
├── 🛠️ Infrastructure Layer (scripts/, env/)
└── 📄 Root Configuration Files
```

---

## 📁 Complete Directory Structure

```
PJMT/
│
├── 📄 README.md                         # Project overview & quick start
├── 📄 SYSTEM_OVERVIEW.md                # Complete system documentation
├── 📄 QUICK_START.md                    # 5-minute setup guide
├── 📄 INTERACTIVE_MODE.md               # Interactive commands guide
├── 📄 FILE_STRUCTURE.md                 # This file
├── 📄 .gitattributes                    # Git LFS and line ending config
│
├── 📂 docs/                             # 📚 Documentation Layer
│   ├── 📄 DOCS_INDEX.md                # Documentation navigation hub
│   │
│   ├── 📂 architecture/                # System Design & Patterns
│   │   ├── ARCHITECTURE_V3.md          # ⭐ Current architecture (v3.0)
│   │   ├── ARCHITECTURE_VISUAL.md      # Visual diagrams
│   │   ├── CLEAN_ARCHITECTURE.md       # ⭐ Clean architecture guide
│   │   ├── SOLID_DESIGN_PATTERNS.md    # ⭐ 7 design patterns
│   │   ├── ARCHITECTURE_IMPROVEMENTS.md # Architecture improvements log
│   │   └── ARCHITECTURE.md             # Legacy architecture (v1.0)
│   │
│   ├── 📂 guides/                      # User & Developer Guides
│   │   ├── INSTALLATION.md             # Setup instructions
│   │   ├── DEVELOPMENT.md              # Development workflow
│   │   ├── MODES_GUIDE.md              # Development vs Production
│   │   └── MIGRATION.md                # Version migration guide
│   │
│   ├── 📂 api/                         # API Documentation
│   │   ├── API_REFERENCE.md            # Complete API specification
│   │   ├── REACT_APP_API.md            # React app integration
│   │   └── QUICK_REFERENCE.md          # Quick API cheatsheet
│   │
│   ├── 📂 history/                     # Change History
│   │   ├── CHANGELOG.md                # Version history
│   │   ├── IMPROVEMENTS_SUMMARY.md     # All improvements made
│   │   ├── TRANSFORMATION_SUMMARY.md   # v2 → v3 transformation
│   │   ├── SIMULATOR_INDEPENDENCE.md   # Simulator redesign
│   │   └── REMOVAL_SUMMARY.md          # Removed features
│   │
│   └── 📂 summaries/                   # Status Reports
│       ├── CHECKLIST.md                # Implementation checklist
│       └── COMPLETE.md                 # Completion report
│
├── 📂 WallboxCtrl/                      # 🎯 Application Layer (C++)
│   │
│   ├── 📂 include/                     # Public header files
│   │   ├── Configuration.h             # Singleton - System config
│   │   ├── GpioFactory.h               # Factory - GPIO creation
│   │   ├── ApiController.h             # Controller - API routing
│   │   ├── Application.h               # Application lifecycle
│   │   ├── WallboxController.h         # Facade - Main controller
│   │   ├── ChargingStateMachine.h      # State - State management
│   │   ├── IGpioController.h           # Interface - GPIO abstraction
│   │   ├── INetworkCommunicator.h      # Interface - Network abstraction
│   │   ├── BananaPiGpioController.h    # Strategy - BananaPi GPIO
│   │   ├── StubGpioController.h        # Strategy - Stub GPIO (testing)
│   │   ├── UdpCommunicator.h           # Strategy - UDP network
│   │   ├── HttpApiServer.h             # Infrastructure - HTTP server
│   │   └── SimpleWallboxController.h   # Legacy - v1.0 controller
│   │
│   ├── 📂 src/                         # Implementation files
│   │   ├── main_v3.cpp                 # v3.0 entry point (current)
│   │   ├── main_v2_with_api.cpp        # v2.0 with API
│   │   ├── main_v2.cpp                 # v2.0 SOLID version
│   │   ├── main.cpp                    # v1.0 legacy
│   │   ├── simulator.cpp               # ISO 15118 simulator
│   │   ├── WallboxController.cpp       # Main controller logic
│   │   ├── ChargingStateMachine.cpp    # State machine implementation
│   │   ├── SimpleWallboxController.cpp # Legacy controller
│   │   ├── BananaPiGpioController.cpp  # BananaPi GPIO impl
│   │   ├── StubGpioController.cpp      # Stub GPIO impl
│   │   ├── UdpCommunicator.cpp         # UDP communication
│   │   ├── HttpApiServer.cpp           # HTTP API server
│   │   └── IsoStackCtrlProtocol_impl.cpp # Protocol implementation
│   │
│   ├── 📂 build/                       # Build artifacts (gitignored)
│   │   ├── wallbox_control_v3          # ⭐ Current version (v3.0)
│   │   ├── wallbox_control_v2          # v2.0 SOLID
│   │   ├── wallbox_control_api         # v2.0 with API
│   │   ├── wallbox_control             # v1.0 legacy
│   │   ├── simulator                   # ISO 15118 simulator
│   │   ├── config.json                 # Runtime configuration
│   │   └── [CMake files]              # Build system files
│   │
│   ├── 📂 tests/                       # Unit & integration tests
│   │   └── [test files]
│   │
│   ├── 📄 CMakeLists.txt               # Build configuration
│   ├── 📄 build.sh                     # Build script
│   ├── 📄 test.sh                      # Test runner
│   ├── 📄 test_interactive.sh          # Interactive test
│   ├── 📄 config.json                  # Default configuration
│   ├── 📄 README.md                    # WallboxCtrl documentation
│   ├── 📄 README_V2.md                 # v2.0 documentation
│   ├── 📄 README_V4.md                 # v4.0 features (future)
│   ├── 📄 V4_COMMANDS.md               # v4.0 commands
│   ├── 📄 CONFIG_GUIDE.md              # Configuration guide
│   ├── 📄 UDP_CONFIG_GUIDE.md          # UDP setup guide
│   └── 📄 INTERACTIVE_GUIDE.md         # Interactive mode guide
│
├── 📂 LibPubWallbox/                    # 🔌 Protocol Layer (ISO 15118)
│   ├── IsoStackCtrlProtocol.h          # Protocol definitions
│   ├── IsoStackCtrlProtocol.cpp        # Protocol implementation
│   ├── Messages.h                      # Message structures
│   ├── Messages.cpp                    # Message handling
│   ├── ChargingProcessTransmitterHTTP_Interface.h
│   ├── ChargingProcessTransmitterHTTP_Interface.cpp
│   ├── test_iso.cpp                    # Protocol tests
│   ├── test_iso                        # Test executable
│   ├── Makefile                        # Build file
│   ├── README_IsoStackCtrl.md          # Protocol documentation
│   │
│   ├── 📂 GruppeC/                     # HTTP transmitter
│   │   ├── ChargingProcessTransmitterHTTP.h
│   │   └── ChargingProcessTransmitterHTTP.cpp
│   │
│   ├── 📂 curl/                        # libcurl dependency
│   │   ├── include/
│   │   └── lib/
│   │
│   ├── 📂 libmicrohttpd/               # HTTP server library
│   │   ├── include/
│   │   └── mingw64/
│   │
│   └── 📂 Dox/                         # Doxygen documentation
│
├── 📂 wallbox-react-app/                # 🌐 Presentation Layer (React)
│   ├── 📂 public/
│   │   └── index.html                  # HTML template
│   │
│   ├── 📂 src/
│   │   ├── App.js                      # Main React component
│   │   ├── App.css                     # Styling
│   │   ├── index.js                    # Entry point
│   │   ├── index.css                   # Global styles
│   │   │
│   │   ├── 📂 api/                     # API integration layer
│   │   │   └── wallboxApi.js          # Wallbox API client
│   │   │
│   │   └── 📂 utils/                   # Utility functions
│   │       └── logger.js              # Logging utility
│   │
│   ├── 📄 package.json                 # Dependencies
│   ├── 📄 README.md                    # React app documentation
│   └── 📄 .gitignore                   # Ignored files
│
├── 📂 scripts/                          # 🛠️ Infrastructure Scripts
│   ├── start-dev.sh                    # Start development mode
│   ├── start-prod.sh                   # Start production mode
│   └── start-api-only.sh               # Start API server only
│
├── 📂 env/                              # 🐳 Deployment Configuration
│   ├── docker-compose.yml              # Docker orchestration
│   ├── Dockerfile                      # Container definition
│   └── README.md                       # Deployment guide
│
└── 📂 .vscode/                          # IDE configuration (optional)
    └── [VS Code settings]
```

---

## 🎯 Layer Responsibilities

### 📚 Documentation Layer (`docs/`)

**Purpose**: All project documentation organized by topic and purpose

| Directory       | Content                                               | Audience                |
| --------------- | ----------------------------------------------------- | ----------------------- |
| `architecture/` | System design, patterns, diagrams, clean architecture | Architects, Senior Devs |
| `guides/`       | Setup, development, migration                         | All Developers          |
| `api/`          | API endpoints, integration                            | Frontend Devs, QA       |
| `history/`      | Changelogs, improvements, transformations             | All Stakeholders        |
| `summaries/`    | Status reports, checklists                            | PM, QA                  |

**New Organization Benefits**:

- ✅ Clear categorization by purpose
- ✅ Easier to find specific documentation
- ✅ Better separation of current vs historical docs
- ✅ Cleaner root directory

### 🎯 Application Layer (`WallboxCtrl/`)

**Purpose**: Core business logic and control system

| Directory  | Content           | Pattern/Purpose       |
| ---------- | ----------------- | --------------------- |
| `include/` | Public interfaces | Interface Segregation |
| `src/`     | Implementations   | Single Responsibility |
| `build/`   | Compiled binaries | Build artifacts       |
| `tests/`   | Test suite        | Quality assurance     |

**Key Files:**

- `main_v3.cpp` - ⭐ **Current production version**
- `wallbox_control_v3` - ⭐ **Current executable**
- `simulator` - ISO 15118 test simulator

### 🔌 Protocol Layer (`LibPubWallbox/`)

**Purpose**: ISO 15118 communication protocol

- Protocol definitions and message structures
- Independent library (can be used by other projects)
- Minimal dependencies on application layer

### 🌐 Presentation Layer (`wallbox-react-app/`)

**Purpose**: User interface and visualization

- Modern React single-page application
- Communicates via HTTP API (port 8080)
- Real-time status updates (2-second polling)

### 🛠️ Infrastructure Layer

**Purpose**: Deployment and automation

- **`scripts/`** - Shell scripts for common tasks
- **`env/`** - Docker and deployment config

---

## 📊 File Categories

### Configuration Files

```
Root Level:
├── .gitattributes              # Git configuration
└── PJMT/WallboxCtrl/
    ├── CMakeLists.txt          # Build system
    ├── config.json             # Runtime config
    ├── CONFIG_GUIDE.md         # Config documentation
    └── UDP_CONFIG_GUIDE.md     # Network config
```

### Documentation Files

```
docs/
├── DOCS_INDEX.md               # 📑 Start here for navigation
├── architecture/               # 🏗️ System design
├── guides/                     # 📖 How-to guides
└── api/                        # 🔌 API reference
```

### Source Code Files

```
WallboxCtrl/
├── include/                    # 📦 Headers (interfaces)
│   ├── Configuration.h         # Singleton pattern
│   ├── WallboxController.h     # Facade pattern
│   ├── ChargingStateMachine.h  # State pattern
│   ├── IGpioController.h       # Strategy interface
│   └── INetworkCommunicator.h  # Strategy interface
│
└── src/                        # 🔨 Implementations
    ├── main_v3.cpp             # ⭐ Current entry point
    ├── WallboxController.cpp   # Main logic
    └── simulator.cpp           # Test simulator
```

### Executable Files

```
WallboxCtrl/build/
├── wallbox_control_v3          # ⭐ Current version (recommended)
├── wallbox_control_v2          # Legacy v2.0
├── wallbox_control_api         # Legacy v2.0 with API
├── wallbox_control             # Legacy v1.0
└── simulator                   # ISO 15118 simulator
```

---

## 🚀 Quick Navigation

### For New Developers

1. **Start**: `README.md` → Overview
2. **Setup**: `docs/guides/INSTALLATION.md` → Get running
3. **Learn**: `docs/architecture/ARCHITECTURE_V3.md` → Understand system
4. **Code**: `WallboxCtrl/src/main_v3.cpp` → See entry point

### For Frontend Developers

1. **API**: `docs/api/API_REFERENCE.md` → All endpoints
2. **Integration**: `docs/api/REACT_APP_API.md` → React integration
3. **Examples**: `wallbox-react-app/src/api/wallboxApi.js` → Client code

### For System Architects

1. **Design**: `docs/architecture/ARCHITECTURE_V3.md` → System design
2. **Patterns**: `docs/SOLID_DESIGN_PATTERNS.md` → Implementation patterns
3. **Visual**: `docs/architecture/ARCHITECTURE_VISUAL.md` → Diagrams

### For DevOps/Deployment

1. **Scripts**: `scripts/` → Automation scripts
2. **Docker**: `env/` → Container configuration
3. **Modes**: `docs/guides/MODES_GUIDE.md` → Dev vs Prod

---

## 🏗️ Architecture Patterns

### Clean Architecture Layers

```
┌─────────────────────────────────────┐
│  Presentation Layer (React App)     │
└──────────────┬──────────────────────┘
               │ HTTP API (port 8080)
┌──────────────┴──────────────────────┐
│  Application Layer (WallboxCtrl)    │
│  - Controllers                       │
│  - Services                          │
│  - State Machine                     │
└──────────────┬──────────────────────┘
               │ Interfaces
┌──────────────┴──────────────────────┐
│  Infrastructure Layer                │
│  - GPIO (Strategy Pattern)           │
│  - Network (Strategy Pattern)        │
│  - Configuration (Singleton)         │
└──────────────┬──────────────────────┘
               │
┌──────────────┴──────────────────────┐
│  Protocol Layer (LibPubWallbox)      │
│  - ISO 15118 Protocol                │
│  - Message Structures                │
└─────────────────────────────────────┘
```

### Dependency Flow

```
React App → HTTP API → WallboxController
                       ↓
              ┌────────┴────────┐
              ↓                 ↓
         StateMachine      IGpioController
                                ↓
                       BananaPiGpioController
```

---

## 📝 Naming Conventions

### Files

- **Headers**: `PascalCase.h` (e.g., `WallboxController.h`)
- **Sources**: `PascalCase.cpp` (e.g., `WallboxController.cpp`)
- **Executables**: `snake_case` (e.g., `wallbox_control_v3`)
- **Scripts**: `kebab-case.sh` (e.g., `start-dev.sh`)
- **Docs**: `SCREAMING_SNAKE_CASE.md` (e.g., `README.md`)

### Directories

- **Code**: `PascalCase/` (e.g., `WallboxCtrl/`)
- **Docs**: `lowercase/` (e.g., `docs/guides/`)

### Classes

- **Interfaces**: `IClassName` (e.g., `IGpioController`)
- **Implementations**: `ClassName` (e.g., `BananaPiGpioController`)
- **Patterns**: `Pattern` suffix when appropriate (e.g., `GpioFactory`)

---

## 🔍 Finding Files Quickly

### By Purpose

| Need                    | File                                   |
| ----------------------- | -------------------------------------- |
| Start developing        | `docs/guides/INSTALLATION.md`          |
| Understand architecture | `docs/architecture/ARCHITECTURE_V3.md` |
| API endpoints           | `docs/api/API_REFERENCE.md`            |
| Run simulator           | `WallboxCtrl/build/simulator`          |
| Run wallbox             | `WallboxCtrl/build/wallbox_control_v3` |
| Configure system        | `WallboxCtrl/config.json`              |
| Web interface           | `wallbox-react-app/src/App.js`         |

### By Role

| Role         | Key Files                           |
| ------------ | ----------------------------------- |
| Backend Dev  | `WallboxCtrl/src/*.cpp`             |
| Frontend Dev | `wallbox-react-app/src/*.js`        |
| Architect    | `docs/architecture/*.md`            |
| QA/Tester    | `WallboxCtrl/test*.sh`, `simulator` |
| DevOps       | `scripts/*.sh`, `env/*`             |

---

## 🎯 Version Information

### Current Version: v3.0

**Main executable**: `wallbox_control_v3`  
**Entry point**: `main_v3.cpp`  
**Architecture**: Clean Architecture with SOLID principles  
**Patterns**: 7 design patterns implemented

### Legacy Versions

- **v2.0**: `wallbox_control_v2` (SOLID principles)
- **v2.0 API**: `wallbox_control_api` (with HTTP API)
- **v1.0**: `wallbox_control` (original implementation)

---

## 📦 Dependencies

### Build Dependencies

- CMake 3.10+
- GCC/G++ with C++14 support
- WiringPi library (for BananaPi GPIO)
- libmicrohttpd (for HTTP API)
- curl library (for HTTP client)

### Runtime Dependencies

- Linux operating system
- Network access (UDP ports 50010, 50011)
- HTTP port 8080 (for API)

### Frontend Dependencies

- Node.js 14+
- npm or yarn
- Modern web browser

---

## 🔒 Git Configuration

### Ignored Files (`.gitignore`)

```
WallboxCtrl/build/          # Build artifacts
node_modules/               # npm dependencies
.DS_Store                   # macOS files
*.o, *.a                    # Compiled objects
```

### Attributes (`.gitattributes`)

```
*.cpp text eol=lf           # Unix line endings for C++
*.h text eol=lf             # Unix line endings for headers
*.sh text eol=lf            # Unix line endings for scripts
```

---

## 📈 Growth Path

### Adding New Features

1. **Interface**: Add to `include/` directory
2. **Implementation**: Add to `src/` directory
3. **Documentation**: Update `docs/` appropriately
4. **Tests**: Add to `tests/` directory
5. **Build**: Update `CMakeLists.txt`

### Adding New Documentation

1. Choose appropriate subdirectory in `docs/`
2. Follow naming convention (CAPS_WITH_UNDERSCORES.md)
3. Update `docs/DOCS_INDEX.md`
4. Cross-reference from related docs

---

## ✅ File Structure Best Practices

1. ✅ **Clear separation** of concerns (layers)
2. ✅ **Consistent naming** conventions
3. ✅ **Logical grouping** by functionality
4. ✅ **Documentation** next to code
5. ✅ **Version control** friendly structure
6. ✅ **Easy navigation** for all roles
7. ✅ **Scalable** for future growth

---

**This structure supports clean architecture, SOLID principles, and professional software development practices.**
│ ├── build/ # Build artifacts
│ ├── tests/ # Unit tests
│ └── CMakeLists.txt
│
├── 📂 wallbox-react-app/ # React web interface
│ ├── src/
│ ├── public/
│ └── package.json
│
├── 📂 LibPubWallbox/ # Protocol library
└── 📂 env/ # Docker environment

````

## Key Improvements

### 1. **Centralized Documentation** 📚

All documentation now in `/docs` with clear categorization:

- **architecture/** - System design and patterns
- **guides/** - User and developer guides
- **api/** - API reference and integration

### 2. **Dedicated Scripts Folder** 🔧

All executable scripts moved to `/scripts/`:

- `start-dev.sh` - Development mode
- `start-prod.sh` - Production mode

### 3. **Clear Root Directory** 🧹

Root directory now contains only:

- README.md (entry point)
- SYSTEM_OVERVIEW.md (complete docs)
- Core project folders
- Configuration files

### 4. **Logical Grouping** 📂

Files grouped by:

- **Purpose** (docs, scripts, source)
- **Type** (architecture, guides, API)
- **Audience** (users, developers, admins)

### 5. **Better Navigation** 🗺️

New documentation index ([docs/DOCS_INDEX.md](docs/DOCS_INDEX.md)):

- Quick links by task
- Categorized documentation
- Recommended reading paths

## Migration from Old Structure

### Updated Paths

| Old Path              | New Path                                | Type   |
| --------------------- | --------------------------------------- | ------ |
| `/ARCHITECTURE_V3.md` | `/docs/architecture/ARCHITECTURE_V3.md` | Doc    |
| `/MODES_GUIDE.md`     | `/docs/guides/MODES_GUIDE.md`           | Guide  |
| `/API_REFERENCE.md`   | `/docs/api/API_REFERENCE.md`            | API    |
| `/start-dev.sh`       | `/scripts/start-dev.sh`                 | Script |
| `/CHANGELOG.md`       | `/docs/CHANGELOG.md`                    | Info   |

### Accessing Documentation

**Before:**

```bash
# Hard to find specific docs
ls *.md  # 15+ files in root
````

**After:**

```bash
# Clear organization
ls docs/architecture/  # Architecture docs
ls docs/guides/       # User guides
ls docs/api/          # API docs
```

### Script Execution

**Before:**

```bash
./start-dev.sh
```

**After:**

```bash
./scripts/start-dev.sh
```

## Benefits

✅ **Easier Navigation** - Find docs by category  
✅ **Cleaner Root** - Less clutter  
✅ **Better Organization** - Logical grouping  
✅ **Professional Structure** - Industry standard  
✅ **Scalable** - Easy to add new docs  
✅ **Git Friendly** - Better .gitattributes

## Documentation Index

See [docs/DOCS_INDEX.md](docs/DOCS_INDEX.md) for complete documentation navigation.

## Quick Access

### Most Used Docs

- **Getting Started:** [README.md](README.md)
- **Modes Guide:** [docs/guides/MODES_GUIDE.md](docs/guides/MODES_GUIDE.md)
- **API Reference:** [docs/api/API_REFERENCE.md](docs/api/API_REFERENCE.md)
- **Architecture:** [docs/architecture/ARCHITECTURE_V3.md](docs/architecture/ARCHITECTURE_V3.md)

### Common Tasks

```bash
# Start development
./scripts/start-dev.sh

# View all docs
ls docs/

# Check architecture
cat docs/architecture/ARCHITECTURE_V3.md

# API reference
cat docs/api/API_REFERENCE.md
```

## Notes

- All internal links updated
- No breaking changes to functionality
- Scripts still work (just moved location)
- Git history preserved
- .gitattributes added for better repo stats
