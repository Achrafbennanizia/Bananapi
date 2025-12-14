# User & Developer Guides

Comprehensive guides for installing, using, and developing the Wallbox Control System.

## 📚 Available Guides

### Getting Started

| Guide                                 | Description                        | Time      | Difficulty    |
| ------------------------------------- | ---------------------------------- | --------- | ------------- |
| [Quick Start](QUICK_START.md)         | Get up and running in 5 minutes    | 5 min     | ⭐ Easy       |
| [Installation Guide](INSTALLATION.md) | Complete installation instructions | 15-30 min | ⭐⭐ Moderate |

### User Guides

| Guide                               | Description                        | Audience       |
| ----------------------------------- | ---------------------------------- | -------------- |
| [Modes Guide](MODES_GUIDE.md)       | Production/Development/Debug modes | Users, DevOps  |
| [UDP Sync Guide](UDP_SYNC_GUIDE.md) | Network communication setup        | Advanced users |

### Developer Guides

| Guide                               | Description                        | Audience           |
| ----------------------------------- | ---------------------------------- | ------------------ |
| [Development Guide](DEVELOPMENT.md) | Development workflow and practices | Developers         |
| [SSH Setup](SSH_SETUP.md)           | SSH key authentication             | DevOps, Developers |
| [Migration Guide](MIGRATION.md)     | Upgrade between versions           | All                |

## 🚀 Quick Navigation

### I want to...

**...install and run the system**
→ Start with [Quick Start Guide](QUICK_START.md)

**...deploy to Raspberry Pi/Banana Pi**
→ See [Installation Guide - Deployment Section](INSTALLATION.md#deployment-to-embedded-systems)

**...contribute code**
→ Read [Development Guide](DEVELOPMENT.md) and [Contributing Guidelines](../../CONTRIBUTING.md)

**...understand build modes**
→ Check [Modes Guide](MODES_GUIDE.md)

**...set up networking**
→ Follow [UDP Sync Guide](UDP_SYNC_GUIDE.md)

**...upgrade from older version**
→ Use [Migration Guide](MIGRATION.md)

## 📖 Guide Summaries

### Quick Start Guide

**What**: 5-minute setup to get the system running  
**Who**: First-time users  
**Prerequisites**: None

**Covers:**

- Quick installation
- First run
- Basic commands
- Test sequence

### Installation Guide

**What**: Complete installation and deployment instructions  
**Who**: Users, operators, DevOps  
**Prerequisites**: Basic Linux knowledge

**Covers:**

- System requirements
- Dependencies
- Build from source
- Deployment to embedded systems
- Configuration
- Troubleshooting

### Modes Guide

**What**: Understanding production, development, and debug modes  
**Who**: All users  
**Prerequisites**: System installed

**Covers:**

- Mode differences
- When to use each mode
- Performance characteristics
- Debugging capabilities
- Mode selection

### UDP Sync Guide

**What**: Setting up network communication between components  
**Who**: Advanced users, system integrators  
**Prerequisites**: Networking knowledge

**Covers:**

- UDP protocol overview
- Port configuration
- Network topology
- Troubleshooting connectivity
- Firewall configuration

### Development Guide

**What**: Development workflow, tools, and practices  
**Who**: Developers, contributors  
**Prerequisites**: C++ knowledge, Git familiarity

**Covers:**

- Development setup
- Build system (CMake)
- Coding standards
- Testing strategies
- Debugging techniques
- Contributing workflow

### SSH Setup Guide

**What**: SSH key-based authentication for deployments  
**Who**: DevOps, developers  
**Prerequisites**: SSH knowledge

**Covers:**

- SSH key generation
- Key deployment
- Configuration
- Security best practices
- Troubleshooting

### Migration Guide

**What**: Upgrading between major versions  
**Who**: Existing users  
**Prerequisites**: Previous version installed

**Covers:**

- Version compatibility
- Breaking changes
- Migration steps
- Data migration
- Rollback procedures

## 🛠️ Common Tasks

### Build the Project

```bash
mkdir build && cd build
cmake -DBUILD_MODE=production ..
make -j$(nproc)
```

See: [Quick Start Guide](QUICK_START.md)

### Run the System

```bash
# Terminal 1: Start simulator
./build/bin/simulator

# Terminal 2: Start wallbox controller
./build/bin/wallbox_control_v4
```

See: [Quick Start Guide](QUICK_START.md#running-the-system)

### Deploy to Device

```bash
./scripts/deploy/install.sh --interactive
```

See: [Installation Guide](INSTALLATION.md#deployment-to-embedded-systems)

### Run Tests

```bash
./scripts/test/test_wallbox.sh
```

See: [Development Guide](DEVELOPMENT.md#testing)

### Change Build Mode

```bash
cd build
cmake -DBUILD_MODE=development ..
make clean && make -j$(nproc)
```

See: [Modes Guide](MODES_GUIDE.md)

## 📊 Guide Difficulty Levels

⭐ **Easy** - No prior knowledge required, follow step-by-step  
⭐⭐ **Moderate** - Basic technical knowledge helpful  
⭐⭐⭐ **Advanced** - Requires technical expertise

## 🎯 Learning Paths

### Path 1: User/Operator

1. [Quick Start Guide](QUICK_START.md) - Get familiar with the system
2. [Installation Guide](INSTALLATION.md) - Learn installation process
3. [Modes Guide](MODES_GUIDE.md) - Understand different modes
4. [UDP Sync Guide](UDP_SYNC_GUIDE.md) - Configure networking

**Time**: 2-3 hours  
**Outcome**: Can install, configure, and operate the system

### Path 2: Developer/Contributor

1. [Quick Start Guide](QUICK_START.md) - Initial setup
2. [Development Guide](DEVELOPMENT.md) - Development environment
3. [Architecture Documentation](../architecture/) - Understand design
4. [Contributing Guidelines](../../CONTRIBUTING.md) - Contribution process

**Time**: 4-6 hours  
**Outcome**: Can contribute code to the project

### Path 3: DevOps/System Administrator

1. [Installation Guide](INSTALLATION.md) - Deployment procedures
2. [SSH Setup Guide](SSH_SETUP.md) - Secure authentication
3. [Modes Guide](MODES_GUIDE.md) - Environment configuration
4. [UDP Sync Guide](UDP_SYNC_GUIDE.md) - Network setup

**Time**: 3-4 hours  
**Outcome**: Can deploy and maintain production systems

## 🆘 Getting Help

### Documentation Issues

- Found an error in a guide? [Open an issue](https://github.com/Achrafbennanizia/Bananapi/issues)
- Have a suggestion? [Start a discussion](https://github.com/Achrafbennanizia/Bananapi/discussions)

### Technical Support

- Check [Troubleshooting sections](INSTALLATION.md#troubleshooting) in guides
- Search [existing issues](https://github.com/Achrafbennanizia/Bananapi/issues)
- Ask in [GitHub Discussions](https://github.com/Achrafbennanizia/Bananapi/discussions)

### Contributing to Guides

See [Contributing Guidelines](../../CONTRIBUTING.md#documentation) for:

- Documentation standards
- Writing style
- Review process

## 📝 Guide Maintenance

### Update Schedule

- **Quick Start**: Updated with minor releases
- **Installation**: Updated with each release
- **Development**: Updated as tooling changes
- **Migration**: Created for major versions

### Version Compatibility

| Guide        | Version  | Status     |
| ------------ | -------- | ---------- |
| Quick Start  | v4.1.0   | ✅ Current |
| Installation | v4.1.0   | ✅ Current |
| Modes Guide  | v4.1.0   | ✅ Current |
| Development  | v4.1.0   | ✅ Current |
| Migration    | v4.0→4.1 | ✅ Current |

## 🔗 Related Documentation

- [Main README](../../README.md) - Project overview
- [Architecture Docs](../architecture/) - System design
- [API Documentation](../api/) - API reference
- [CHANGELOG](../../CHANGELOG.md) - Version history

## 📚 External Resources

### Standards

- [ISO 15118](https://www.iso.org/standard/55366.html) - EV communication protocol
- [IEC 61851-1](https://webstore.iec.ch/publication/6029) - EV charging system

### Tools

- [CMake Documentation](https://cmake.org/documentation/)
- [GCC Manual](https://gcc.gnu.org/onlinedocs/)
- [Git Book](https://git-scm.com/book/en/v2)

### Best Practices

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Clean Code](https://www.oreilly.com/library/view/clean-code-a/9780136083238/)
- [Test-Driven Development](https://martinfowler.com/bliki/TestDrivenDevelopment.html)

---

_Guides are maintained by the community. Last updated: v4.1.0 (December 2024)_

**Need a new guide?** [Request it here](https://github.com/Achrafbennanizia/Bananapi/issues/new?template=documentation_request.md)
