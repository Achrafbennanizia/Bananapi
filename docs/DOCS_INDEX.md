# Documentation Index

Welcome to the ISO 15118 Wallbox Control System documentation!

## 📚 Documentation Structure

### 🎯 Start Here

1. **[README.md](README.md)** (20 KB) - **START HERE**

   - Project overview and features
   - System architecture diagrams
   - Quick start guide
   - Hardware support information
   - Troubleshooting tips

2. **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** (4.3 KB) - **Cheat Sheet**
   - One-liner installation
   - Essential commands
   - Common workflows
   - Quick troubleshooting

### 🔧 Setup & Installation

3. **[INSTALLATION.md](INSTALLATION.md)** (8.3 KB)
   - Detailed installation instructions
   - Platform-specific guides (Ubuntu, Raspberry Pi, macOS, Windows)
   - Post-installation configuration
   - Systemd service setup
   - Docker installation

### 📖 Technical Documentation

4. **[API_REFERENCE.md](API_REFERENCE.md)** (16 KB)

   - Complete protocol specification
   - Data structure definitions
   - Message format details
   - Network protocol documentation
   - Code examples

5. **[WallboxCtrl/README.md](WallboxCtrl/README.md)** (Component-specific)

   - Wallbox controller details
   - Simulator documentation
   - Build instructions
   - Usage examples

6. **[WallboxCtrl/INTERACTIVE_GUIDE.md](WallboxCtrl/INTERACTIVE_GUIDE.md)**
   - Step-by-step interactive tutorial
   - Test scenarios
   - Command walkthroughs

### 👨‍💻 Development

7. **[DEVELOPMENT.md](DEVELOPMENT.md)** (12 KB)

   - Development environment setup
   - Coding standards
   - Build from source
   - Testing guide
   - Contribution workflow
   - Release process

8. **[CHANGELOG.md](CHANGELOG.md)** (5.1 KB)
   - Version history
   - Release notes
   - Migration guides
   - Breaking changes

### 📦 Additional Resources

9. **[env/README.md](env/README.md)**

   - Docker environment
   - Container setup
   - Docker Compose configuration

10. **[LibPubWallbox/README_IsoStackCtrl.md](LibPubWallbox/README_IsoStackCtrl.md)**
    - ISO protocol library
    - Protocol internals
    - Library API

## 🗺️ Documentation Map by Use Case

### I want to...

#### **Get started quickly**

→ [README.md](README.md#quick-start) → [QUICK_REFERENCE.md](QUICK_REFERENCE.md)

#### **Install on my system**

→ [INSTALLATION.md](INSTALLATION.md) → Platform-specific section

#### **Learn the commands**

→ [QUICK_REFERENCE.md](QUICK_REFERENCE.md#essential-commands) → [WallboxCtrl/README.md](WallboxCtrl/README.md)

#### **Understand the protocol**

→ [API_REFERENCE.md](API_REFERENCE.md) → [LibPubWallbox/README_IsoStackCtrl.md](LibPubWallbox/README_IsoStackCtrl.md)

#### **Contribute code**

→ [DEVELOPMENT.md](DEVELOPMENT.md) → [CHANGELOG.md](CHANGELOG.md)

#### **Troubleshoot issues**

→ [README.md#troubleshooting](README.md#troubleshooting) → [INSTALLATION.md](INSTALLATION.md#troubleshooting-installation)

#### **Deploy with Docker**

→ [env/README.md](env/README.md) → [INSTALLATION.md#method-3-docker-installation](INSTALLATION.md#method-3-docker-installation)

#### **Set up hardware (Raspberry Pi)**

→ [README.md#hardware-support](README.md#hardware-support) → [INSTALLATION.md#raspberry-pi-os](INSTALLATION.md#raspberry-pi-os)

#### **Understand architecture**

→ [README.md#system-architecture](README.md#system-architecture) → [API_REFERENCE.md#protocol-overview](API_REFERENCE.md#protocol-overview)

#### **Run tests**

→ [DEVELOPMENT.md#testing](DEVELOPMENT.md#testing) → [WallboxCtrl/test_interactive.sh](WallboxCtrl/test_interactive.sh)

## 📊 Documentation Statistics

| Document           | Size      | Lines      | Purpose          |
| ------------------ | --------- | ---------- | ---------------- |
| README.md          | 20 KB     | ~650       | Main entry point |
| API_REFERENCE.md   | 16 KB     | ~600       | Protocol docs    |
| DEVELOPMENT.md     | 12 KB     | ~500       | Dev guide        |
| INSTALLATION.md    | 8.3 KB    | ~400       | Setup guide      |
| CHANGELOG.md       | 5.1 KB    | ~230       | Version history  |
| QUICK_REFERENCE.md | 4.3 KB    | ~220       | Cheat sheet      |
| **Total**          | **66 KB** | **~2,600** | Complete docs    |

## 🔍 Search Tips

### Find Specific Information

**Installation on Ubuntu:**

```bash
grep -n "Ubuntu" INSTALLATION.md
```

**Message structure definitions:**

```bash
grep -A 10 "struct st" API_REFERENCE.md
```

**All commands:**

```bash
grep -E "Commands:|Command \|" *.md
```

**Error solutions:**

```bash
grep -B 2 -A 5 "Error:" INSTALLATION.md
```

## 📱 Quick Navigation

### By Reader Type

**🆕 New Users:**

1. [README.md](README.md) - Overview
2. [INSTALLATION.md](INSTALLATION.md) - Setup
3. [QUICK_REFERENCE.md](QUICK_REFERENCE.md) - Commands

**👨‍💻 Developers:**

1. [DEVELOPMENT.md](DEVELOPMENT.md) - Dev setup
2. [API_REFERENCE.md](API_REFERENCE.md) - API docs
3. [CHANGELOG.md](CHANGELOG.md) - History

**🔌 Hardware Engineers:**

1. [README.md#hardware-support](README.md#hardware-support) - GPIO
2. [INSTALLATION.md#raspberry-pi-os](INSTALLATION.md#raspberry-pi-os) - Pi setup
3. [API_REFERENCE.md#gpio-configuration](API_REFERENCE.md#gpio-configuration) - Pin config

**🎓 Researchers:**

1. [API_REFERENCE.md](API_REFERENCE.md) - Protocol
2. [README.md#system-architecture](README.md#system-architecture) - Architecture
3. [LibPubWallbox/README_IsoStackCtrl.md](LibPubWallbox/README_IsoStackCtrl.md) - Library

## 🆘 Getting Help

### Documentation Feedback

Found an issue with the docs? Please:

1. **Check if already reported:** [GitHub Issues](https://github.com/Achrafbennanizia/Bananapi/issues)
2. **Create new issue:** Use label `documentation`
3. **Suggest improvements:** Pull requests welcome!

### Support Channels

- 📖 **Documentation:** You're here!
- 🐛 **Bug Reports:** [GitHub Issues](https://github.com/Achrafbennanizia/Bananapi/issues)
- 💬 **Questions:** [GitHub Discussions](https://github.com/Achrafbennanizia/Bananapi/discussions)
- 📧 **Email:** achraf.bennani@example.com

## 🔄 Document Updates

This documentation is maintained alongside the code. Last updated: **2025-12-08**

Check for updates:

```bash
git pull origin main
```

## 📝 Contributing to Docs

See [DEVELOPMENT.md#contributing](DEVELOPMENT.md#contributing) for:

- Documentation style guide
- How to submit improvements
- Building documentation locally

## 🌐 Online Documentation

- **GitHub Repository:** https://github.com/Achrafbennanizia/Bananapi
- **GitHub Wiki:** https://github.com/Achrafbennanizia/Bananapi/wiki
- **GitHub Pages:** (Coming soon)

## 📄 License

All documentation is licensed under [MIT License](LICENSE) - same as the code.

---

**Ready to get started?** → [README.md](README.md) 🚀
