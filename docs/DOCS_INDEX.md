# 📑 Documentation Index - Clean Architecture

**Last Updated**: December 10, 2025  
**Version**: 3.0  
**Documentation Status**: Production Ready

Welcome to the ISO 15118 Wallbox Control System documentation! This index follows a **role-based navigation** approach for quick access.

---

## 🚀 Quick Start (5 Minutes)

| Document                                            | Purpose                     | Time  |
| --------------------------------------------------- | --------------------------- | ----- |
| **[📄 ../README.md](../README.md)**                 | Project overview & features | 2 min |
| **[⚡ ../QUICK_START.md](../QUICK_START.md)**       | Setup in 5 minutes          | 5 min |
| **[📂 ../FILE_STRUCTURE.md](../FILE_STRUCTURE.md)** | Navigate the codebase       | 3 min |

---

## 👥 Navigation by Role

### 🎓 New Developers (Start Here!)

Follow this path for fastest onboarding:

1. **[../README.md](../README.md)** - Project overview
2. **[guides/INSTALLATION.md](guides/INSTALLATION.md)** - Get it running
3. **[../QUICK_START.md](../QUICK_START.md)** - 5-minute tutorial
4. **[architecture/ARCHITECTURE_V3.md](architecture/ARCHITECTURE_V3.md)** - Understand the system
5. **[architecture/SOLID_DESIGN_PATTERNS.md](architecture/SOLID_DESIGN_PATTERNS.md)** - Code patterns used

**Estimated time**: 30 minutes to productive development

---

### 💻 Backend Developers (C++)

Working on WallboxCtrl? Start here:

| Document                                                                           | Content                       | Priority    |
| ---------------------------------------------------------------------------------- | ----------------------------- | ----------- |
| **[architecture/ARCHITECTURE_V3.md](architecture/ARCHITECTURE_V3.md)**             | System design & state machine | ⭐ Critical |
| **[architecture/SOLID_DESIGN_PATTERNS.md](architecture/SOLID_DESIGN_PATTERNS.md)** | 7 design patterns reference   | ⭐ Critical |
| **[../WallboxCtrl/README.md](../WallboxCtrl/README.md)**                           | Component documentation       | High        |
| **[../WallboxCtrl/CONFIG_GUIDE.md](../WallboxCtrl/CONFIG_GUIDE.md)**               | Configuration system          | High        |
| **[guides/DEVELOPMENT.md](guides/DEVELOPMENT.md)**                                 | Development workflow          | Medium      |

**Key Concepts**: State Machine, Strategy Pattern, Dependency Injection, SOLID Principles

---

### 🎨 Frontend Developers (React)

Building the web interface? Start here:

| Document                                                             | Content                 | Priority    |
| -------------------------------------------------------------------- | ----------------------- | ----------- |
| **[api/REACT_APP_API.md](api/REACT_APP_API.md)**                     | React integration guide | ⭐ Critical |
| **[api/API_REFERENCE.md](api/API_REFERENCE.md)**                     | Complete API endpoints  | ⭐ Critical |
| **[api/QUICK_REFERENCE.md](api/QUICK_REFERENCE.md)**                 | API cheat sheet         | High        |
| **[../wallbox-react-app/README.md](../wallbox-react-app/README.md)** | React app documentation | Medium      |

**Key Endpoints**: GET /status, POST /command, WebSocket (future)

---

### 🏗️ System Architects

Designing features or understanding system? Start here:

| Document                                                                           | Content                       | Priority    |
| ---------------------------------------------------------------------------------- | ----------------------------- | ----------- |
| **[architecture/ARCHITECTURE_V3.md](architecture/ARCHITECTURE_V3.md)**             | Complete system architecture  | ⭐ Critical |
| **[architecture/ARCHITECTURE_VISUAL.md](architecture/ARCHITECTURE_VISUAL.md)**     | Visual diagrams               | ⭐ Critical |
| **[architecture/SOLID_DESIGN_PATTERNS.md](architecture/SOLID_DESIGN_PATTERNS.md)** | Design pattern implementation | ⭐ Critical |
| **[../SYSTEM_OVERVIEW.md](../SYSTEM_OVERVIEW.md)**                                 | High-level system view        | High        |
| **[architecture/ARCHITECTURE.md](architecture/ARCHITECTURE.md)**                   | Legacy v1.0 architecture      | Reference   |

**Focus**: Clean Architecture, SOLID Principles, Design Patterns

---

### 🧪 QA Engineers & Testers

Testing the system? Start here:

| Document                                                                     | Content               | Priority    |
| ---------------------------------------------------------------------------- | --------------------- | ----------- |
| **[../WallboxCtrl/test.sh](../WallboxCtrl/test.sh)**                         | Automated test runner | ⭐ Critical |
| **[../WallboxCtrl/test_interactive.sh](../WallboxCtrl/test_interactive.sh)** | Interactive testing   | ⭐ Critical |
| **[../INTERACTIVE_MODE.md](../INTERACTIVE_MODE.md)**                         | Simulator commands    | High        |
| **[api/API_REFERENCE.md](api/API_REFERENCE.md)**                             | API testing reference | High        |
| **[CHECKLIST.md](CHECKLIST.md)**                                             | Feature checklist     | Medium      |

**Tools**: simulator, curl, Postman, automated tests

---

### 🚀 DevOps Engineers

Deploying the system? Start here:

| Document                                                             | Content                   | Priority    |
| -------------------------------------------------------------------- | ------------------------- | ----------- |
| **[guides/INSTALLATION.md](guides/INSTALLATION.md)**                 | Installation guide        | ⭐ Critical |
| **[guides/MODES_GUIDE.md](guides/MODES_GUIDE.md)**                   | Development vs Production | ⭐ Critical |
| **[../env/README.md](../env/README.md)**                             | Docker deployment         | High        |
| **[../scripts/start-prod.sh](../scripts/start-prod.sh)**             | Production startup        | High        |
| **[../WallboxCtrl/CONFIG_GUIDE.md](../WallboxCtrl/CONFIG_GUIDE.md)** | Configuration management  | Medium      |

**Deployment**: Docker, systemd, bare metal

---

## 📚 Documentation by Category

### 🏗️ Architecture & Design

| Document                                                                                   | Description                    | Audience     |
| ------------------------------------------------------------------------------------------ | ------------------------------ | ------------ |
| **[architecture/ARCHITECTURE_V3.md](architecture/ARCHITECTURE_V3.md)**                     | ⭐ Current architecture (v3.0) | All          |
| **[architecture/ARCHITECTURE_VISUAL.md](architecture/ARCHITECTURE_VISUAL.md)**             | Diagrams & visualizations      | Architects   |
| **[architecture/SOLID_DESIGN_PATTERNS.md](architecture/SOLID_DESIGN_PATTERNS.md)**         | 7 design patterns documented   | Backend Devs |
| **[architecture/CLEAN_ARCHITECTURE.md](architecture/CLEAN_ARCHITECTURE.md)**               | Clean architecture guide       | All          |
| **[architecture/ARCHITECTURE_IMPROVEMENTS.md](architecture/ARCHITECTURE_IMPROVEMENTS.md)** | Architecture improvements      | Architects   |
| **[architecture/ARCHITECTURE.md](architecture/ARCHITECTURE.md)**                           | Legacy v1.0 architecture       | Reference    |
| **[../SYSTEM_OVERVIEW.md](../SYSTEM_OVERVIEW.md)**                                         | High-level system overview     | Stakeholders |

**Key Topics**: State Machine, Clean Architecture, SOLID, Design Patterns

---

### 📖 User Guides

| Document                                             | Description                 | Audience    |
| ---------------------------------------------------- | --------------------------- | ----------- |
| **[guides/INSTALLATION.md](guides/INSTALLATION.md)** | Complete setup instructions | All         |
| **[guides/DEVELOPMENT.md](guides/DEVELOPMENT.md)**   | Development workflow        | Developers  |
| **[guides/MODES_GUIDE.md](guides/MODES_GUIDE.md)**   | Dev vs Production modes     | DevOps      |
| **[guides/MIGRATION.md](guides/MIGRATION.md)**       | Version migration guide     | Maintainers |
| **[../QUICK_START.md](../QUICK_START.md)**           | 5-minute quickstart         | New Users   |

---

### 🔌 API Documentation

| Document                                             | Description                | Audience     |
| ---------------------------------------------------- | -------------------------- | ------------ |
| **[api/API_REFERENCE.md](api/API_REFERENCE.md)**     | Complete API specification | All Devs     |
| **[api/REACT_APP_API.md](api/REACT_APP_API.md)**     | React integration guide    | Frontend     |
| **[api/QUICK_REFERENCE.md](api/QUICK_REFERENCE.md)** | API cheat sheet            | Quick Lookup |

**API Details**: HTTP endpoints (port 8080), UDP protocol (ports 50010/50011)

---

### 📝 History & Changes

| Document                                                                   | Description              | Audience     |
| -------------------------------------------------------------------------- | ------------------------ | ------------ |
| **[history/CHANGELOG.md](history/CHANGELOG.md)**                           | Version history          | All          |
| **[history/IMPROVEMENTS_SUMMARY.md](history/IMPROVEMENTS_SUMMARY.md)**     | All improvements made    | Stakeholders |
| **[history/TRANSFORMATION_SUMMARY.md](history/TRANSFORMATION_SUMMARY.md)** | v2 → v3 transformation   | Architects   |
| **[history/SIMULATOR_INDEPENDENCE.md](history/SIMULATOR_INDEPENDENCE.md)** | Simulator design changes | Backend Devs |
| **[history/REMOVAL_SUMMARY.md](history/REMOVAL_SUMMARY.md)**               | Removed features         | Migration    |

---

### ✅ Status & Completion

| Document                                             | Description              | Audience     |
| ---------------------------------------------------- | ------------------------ | ------------ |
| **[summaries/CHECKLIST.md](summaries/CHECKLIST.md)** | Implementation checklist | PM, QA       |
| **[summaries/COMPLETE.md](summaries/COMPLETE.md)**   | Completion report        | Stakeholders |

---

## 🗺️ Documentation Map by Task

Quick shortcuts for common tasks:

### Getting Started

| Task                  | Documents                                                                                 | Time   |
| --------------------- | ----------------------------------------------------------------------------------------- | ------ |
| **Quick overview**    | [../README.md](../README.md)                                                              | 2 min  |
| **Install & run**     | [guides/INSTALLATION.md](guides/INSTALLATION.md) → [../QUICK_START.md](../QUICK_START.md) | 10 min |
| **First development** | [guides/DEVELOPMENT.md](guides/DEVELOPMENT.md)                                            | 15 min |

### Development Tasks

| Task                | Documents                                                                                                                                           | Priority |
| ------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------- | -------- |
| **Add new feature** | [architecture/ARCHITECTURE_V3.md](architecture/ARCHITECTURE_V3.md) → [architecture/SOLID_DESIGN_PATTERNS.md](architecture/SOLID_DESIGN_PATTERNS.md) | ⭐       |
| **Fix a bug**       | [guides/DEVELOPMENT.md](guides/DEVELOPMENT.md) → Component README                                                                                   | High     |
| **Write tests**     | [../WallboxCtrl/tests/](../WallboxCtrl/tests/) → [../WallboxCtrl/test.sh](../WallboxCtrl/test.sh)                                                   | High     |
| **Update docs**     | This file → Relevant doc                                                                                                                            | Medium   |

### Integration Tasks

| Task                     | Documents                                                                                                                       | Priority |
| ------------------------ | ------------------------------------------------------------------------------------------------------------------------------- | -------- |
| **Frontend integration** | [api/REACT_APP_API.md](api/REACT_APP_API.md) → [api/API_REFERENCE.md](api/API_REFERENCE.md)                                     | ⭐       |
| **Add API endpoint**     | [api/API_REFERENCE.md](api/API_REFERENCE.md) → [../WallboxCtrl/include/ApiController.h](../WallboxCtrl/include/ApiController.h) | High     |
| **Protocol changes**     | [../LibPubWallbox/README_IsoStackCtrl.md](../LibPubWallbox/README_IsoStackCtrl.md)                                              | High     |

### Deployment Tasks

| Task                  | Documents                                                                                             | Priority |
| --------------------- | ----------------------------------------------------------------------------------------------------- | -------- |
| **Production deploy** | [guides/MODES_GUIDE.md](guides/MODES_GUIDE.md) → [../scripts/start-prod.sh](../scripts/start-prod.sh) | ⭐       |
| **Docker setup**      | [../env/README.md](../env/README.md) → [../env/docker-compose.yml](../env/docker-compose.yml)         | High     |
| **Configuration**     | [../WallboxCtrl/CONFIG_GUIDE.md](../WallboxCtrl/CONFIG_GUIDE.md)                                      | High     |

### Troubleshooting

| Task               | Documents                                                                                         | Priority |
| ------------------ | ------------------------------------------------------------------------------------------------- | -------- |
| **Build errors**   | [guides/INSTALLATION.md](guides/INSTALLATION.md) → [guides/DEVELOPMENT.md](guides/DEVELOPMENT.md) | ⭐       |
| **Runtime errors** | [../INTERACTIVE_MODE.md](../INTERACTIVE_MODE.md) → [api/API_REFERENCE.md](api/API_REFERENCE.md)   | High     |
| **Network issues** | [../WallboxCtrl/UDP_CONFIG_GUIDE.md](../WallboxCtrl/UDP_CONFIG_GUIDE.md)                          | Medium   |

---

## 📊 Documentation Completeness

| Category      | Coverage | Status               |
| ------------- | -------- | -------------------- |
| Architecture  | 95%      | ✅ Excellent         |
| API Reference | 100%     | ✅ Complete          |
| User Guides   | 90%      | ✅ Good              |
| Code Examples | 85%      | ✅ Good              |
| Deployment    | 90%      | ✅ Good              |
| Testing       | 80%      | ⚠️ Needs improvement |

---

## 🔄 Version History

| Version | Date     | Major Changes                                           |
| ------- | -------- | ------------------------------------------------------- |
| **3.0** | Dec 2025 | Clean Architecture, SOLID principles, 7 design patterns |
| 2.0     | Nov 2025 | SOLID refactoring, HTTP API added                       |
| 1.0     | Oct 2025 | Initial release                                         |

**Current**: v3.0 (Production Ready)  
**Next**: v4.0 (WebSocket support planned)

---

## 📁 File Organization

```
docs/
├── 📑 DOCS_INDEX.md                      # ⭐ This file (start here)
│
├── 📂 architecture/                     # System Design & Patterns
│   ├── ARCHITECTURE_V3.md               # ⭐ Current architecture (v3.0)
│   ├── ARCHITECTURE_VISUAL.md           # Visual diagrams
│   ├── CLEAN_ARCHITECTURE.md            # ⭐ Clean architecture guide
│   ├── SOLID_DESIGN_PATTERNS.md         # ⭐ 7 design patterns
│   ├── ARCHITECTURE_IMPROVEMENTS.md     # Architecture improvements log
│   └── ARCHITECTURE.md                  # Legacy v1.0 architecture
│
├── 📂 guides/                           # User & Developer Guides
│   ├── INSTALLATION.md                  # Setup instructions
│   ├── DEVELOPMENT.md                   # Development workflow
│   ├── MODES_GUIDE.md                   # Dev vs Production
│   └── MIGRATION.md                     # Version migration
│
├── 📂 api/                              # API Documentation
│   ├── API_REFERENCE.md                 # Complete API specification
│   ├── REACT_APP_API.md                 # React integration guide
│   └── QUICK_REFERENCE.md               # Quick API lookup
│
├── 📂 history/                          # Change History
│   ├── CHANGELOG.md                     # Version history
│   ├── IMPROVEMENTS_SUMMARY.md          # All improvements
│   ├── TRANSFORMATION_SUMMARY.md        # v2 → v3 transformation
│   ├── SIMULATOR_INDEPENDENCE.md        # Simulator redesign
│   └── REMOVAL_SUMMARY.md               # Removed features
│
└── 📂 summaries/                        # Status Reports
    ├── CHECKLIST.md                     # Implementation checklist
    └── COMPLETE.md                      # Completion report
```
---

## 🎯 Best Practices

### For Documentation Writers

1. ✅ Use clear, descriptive titles
2. ✅ Include code examples
3. ✅ Link to related docs
4. ✅ Update DOCS_INDEX.md
5. ✅ Follow markdown best practices

### For Code Contributors

1. ✅ Update relevant docs when changing code
2. ✅ Add comments for complex logic
3. ✅ Update API_REFERENCE.md for API changes
4. ✅ Update CHANGELOG.md for releases

---

## 📞 Getting Help

| Question Type             | Resource                                                           |
| ------------------------- | ------------------------------------------------------------------ |
| **How do I...?**          | Search this index or [guides/](guides/)                            |
| **What does X mean?**     | [architecture/ARCHITECTURE_V3.md](architecture/ARCHITECTURE_V3.md) |
| **API endpoint details?** | [api/API_REFERENCE.md](api/API_REFERENCE.md)                       |
| **Build/install issues?** | [guides/INSTALLATION.md](guides/INSTALLATION.md)                   |
| **Contributing?**         | [guides/DEVELOPMENT.md](guides/DEVELOPMENT.md)                     |

---

## 🌟 Documentation Quality Standards

This documentation follows:

- ✅ **Clear hierarchy** (role-based navigation)
- ✅ **Consistent formatting** (tables, headings, lists)
- ✅ **Up-to-date** (version 3.0)
- ✅ **Cross-referenced** (extensive linking)
- ✅ **Example-rich** (code samples)
- ✅ **Audience-aware** (role-specific sections)
- ✅ **Searchable** (descriptive titles)

---

**🎯 Still lost? Start with [../README.md](../README.md) or [../QUICK_START.md](../QUICK_START.md)**

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
````

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
