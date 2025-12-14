# Documentation Index

Welcome to the Wallbox Control System documentation. This index provides quick access to all project documentation.

## 📚 Documentation Structure

```
docs/
├── README.md                    # This file
├── api/                         # API Documentation
├── architecture/                # System Design & Architecture
├── diagrams/                    # UML and System Diagrams
├── doxygen/                     # Generated API Documentation
├── guides/                      # User & Developer Guides
└── history/                     # Project History & Changelogs
```

## 🚀 Quick Start

**New to the project?** Start here:

1. [Project README](../README.md) - Overview and features
2. [Quick Start Guide](guides/QUICK_START.md) - Get up and running in 5 minutes
3. [Installation Guide](guides/INSTALLATION.md) - Detailed installation instructions
4. [File Structure](FILE_STRUCTURE.md) - Understand the project layout

## 📖 Main Documentation

### Getting Started

| Document                                     | Description                        |
| -------------------------------------------- | ---------------------------------- |
| [Quick Start](guides/QUICK_START.md)         | 5-minute setup and first run       |
| [Installation Guide](guides/INSTALLATION.md) | Complete installation instructions |
| [System Overview](SYSTEM_OVERVIEW.md)        | High-level system architecture     |
| [File Structure](FILE_STRUCTURE.md)          | Project organization explained     |

### User Guides

| Document                                   | Description                           |
| ------------------------------------------ | ------------------------------------- |
| [Modes Guide](guides/MODES_GUIDE.md)       | Production/Development/Debug modes    |
| [UDP Sync Guide](guides/UDP_SYNC_GUIDE.md) | Network communication setup           |
| [Migration Guide](guides/MIGRATION.md)     | Upgrade from previous versions        |
| [SSH Setup](guides/SSH_SETUP.md)           | SSH key authentication for deployment |

### Developer Guides

| Document                                    | Description                        |
| ------------------------------------------- | ---------------------------------- |
| [Development Guide](guides/DEVELOPMENT.md)  | Development workflow and practices |
| [Contributing](../CONTRIBUTING.md)          | How to contribute to the project   |
| [Doxygen API Docs](doxygen/html/index.html) | Complete API reference (588 pages) |

### Architecture Documentation

| Document                                                         | Description                        |
| ---------------------------------------------------------------- | ---------------------------------- |
| [Architecture Overview](architecture/ARCHITECTURE_V3.md)         | Current system architecture (v4.1) |
| [Clean Architecture](architecture/CLEAN_ARCHITECTURE.md)         | Clean architecture implementation  |
| [SOLID & Design Patterns](architecture/SOLID_DESIGN_PATTERNS.md) | Design principles used             |
| [Class Architecture](architecture/CLASS_ARCHITECTURE.md)         | Class structure and relationships  |
| [Visual Architecture](architecture/ARCHITECTURE_VISUAL.md)       | Architecture diagrams              |

### API Documentation

| Document                                  | Description                      |
| ----------------------------------------- | -------------------------------- |
| [API Reference](api/API_REFERENCE.md)     | Complete HTTP REST API reference |
| [Quick Reference](api/QUICK_REFERENCE.md) | Common API commands              |
| [React App API](api/REACT_APP_API.md)     | React dashboard integration      |

### Diagrams

| Document                                           | Description             |
| -------------------------------------------------- | ----------------------- |
| [Class Diagram](diagrams/CLASS_DIAGRAM.md)         | UML class relationships |
| [Component Diagram](diagrams/COMPONENT_DIAGRAM.md) | System components       |
| [Object Diagram](diagrams/OBJECT_DIAGRAM.md)       | Object interactions     |

### Project History

| Document                                                    | Description                 |
| ----------------------------------------------------------- | --------------------------- |
| [Changelog](../CHANGELOG.md)                                | Version history and changes |
| [Reorganization Summary](REORGANIZATION_SUMMARY.md)         | v4.1 restructuring details  |
| [Improvements Summary](history/IMPROVEMENTS_SUMMARY.md)     | Historical improvements     |
| [Transformation Summary](history/TRANSFORMATION_SUMMARY.md) | Architecture evolution      |

## 🔍 Find What You Need

### By Topic

**Installation & Setup**

- [Installation Guide](guides/INSTALLATION.md)
- [Quick Start](guides/QUICK_START.md)
- [SSH Setup](guides/SSH_SETUP.md)

**Development**

- [Development Guide](guides/DEVELOPMENT.md)
- [Contributing Guidelines](../CONTRIBUTING.md)
- [File Structure](FILE_STRUCTURE.md)

**Architecture & Design**

- [System Overview](SYSTEM_OVERVIEW.md)
- [Architecture V3](architecture/ARCHITECTURE_V3.md)
- [SOLID & Design Patterns](architecture/SOLID_DESIGN_PATTERNS.md)

**API & Integration**

- [API Reference](api/API_REFERENCE.md)
- [React App Integration](api/REACT_APP_API.md)
- [UDP Communication](guides/UDP_SYNC_GUIDE.md)

**Troubleshooting**

- [Installation Guide - Troubleshooting](guides/INSTALLATION.md#troubleshooting)
- [Development Guide - Common Issues](guides/DEVELOPMENT.md#troubleshooting)
- [Migration Guide](guides/MIGRATION.md)

### By Role

**Users / Operators**

- Start: [Quick Start Guide](guides/QUICK_START.md)
- Install: [Installation Guide](guides/INSTALLATION.md)
- Configure: [Modes Guide](guides/MODES_GUIDE.md)
- Deploy: [SSH Setup](guides/SSH_SETUP.md)

**Developers**

- Overview: [System Overview](SYSTEM_OVERVIEW.md)
- Setup: [Development Guide](guides/DEVELOPMENT.md)
- Contribute: [Contributing Guidelines](../CONTRIBUTING.md)
- API: [Doxygen Documentation](doxygen/html/index.html)

**Architects / Maintainers**

- Design: [Architecture Documentation](architecture/)
- Patterns: [SOLID & Design Patterns](architecture/SOLID_DESIGN_PATTERNS.md)
- Evolution: [Transformation Summary](history/TRANSFORMATION_SUMMARY.md)
- Structure: [File Structure](FILE_STRUCTURE.md)

## 🛠️ Common Tasks

### Build and Run

```bash
# Quick build
mkdir build && cd build
cmake -DBUILD_MODE=production ..
make -j$(nproc)

# Run
./bin/wallbox_control_v4
```

See: [Quick Start Guide](guides/QUICK_START.md)

### Deploy to Embedded System

```bash
./scripts/deploy/install.sh --interactive
```

See: [Installation Guide](guides/INSTALLATION.md#deployment-to-embedded-systems)

### Run Tests

```bash
./scripts/test/test_wallbox.sh
```

See: [Development Guide](guides/DEVELOPMENT.md#testing)

### Generate API Documentation

```bash
doxygen Doxyfile
open docs/doxygen/html/index.html
```

See: [Doxygen README](doxygen/README.md)

## 📊 Documentation Statistics

- **Total Documents**: 40+
- **API Documentation**: 588 HTML pages
- **Code Examples**: 100+
- **Diagrams**: 10+
- **Guides**: 15+

## 🤝 Contributing to Documentation

Found an error or want to improve the docs? See [Contributing Guidelines](../CONTRIBUTING.md#documentation).

### Documentation Standards

- **Markdown**: Use GitHub Flavored Markdown
- **Code Blocks**: Always specify language
- **Links**: Use relative links for internal docs
- **Images**: Store in `docs/images/` (if needed)
- **Diagrams**: Use Mermaid or PlantUML when possible

### Style Guide

- **Headings**: Use sentence case
- **Lists**: Use `-` for unordered, `1.` for ordered
- **Emphasis**: Use **bold** for UI elements, `code` for commands
- **Line Length**: Aim for 80-120 characters
- **Structure**: Follow existing document templates

## 📞 Getting Help

- **Questions**: Open a [GitHub Discussion](https://github.com/Achrafbennanizia/Bananapi/discussions)
- **Issues**: Report bugs in [GitHub Issues](https://github.com/Achrafbennanizia/Bananapi/issues)
- **Documentation**: Check this index first

## 🔄 Documentation Maintenance

This documentation is maintained by the project team and community contributors. Last major update: v4.1.0 (December 2024).

**Update Frequency**:

- **Guides**: Updated with each minor release
- **API Docs**: Regenerated with each release
- **Architecture**: Updated with significant changes
- **Changelog**: Updated with every release

## 📝 License

Documentation is licensed under the same license as the project. See [LICENSE](../LICENSE) for details.

---

_For the most up-to-date information, always refer to the main README.md and CHANGELOG.md_
