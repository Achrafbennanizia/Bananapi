# 📂 Improved File Structure

The project has been reorganized into a cleaner, more professional structure.

## New Organization

```
PJMT/
├── 📄 README.md                 # Main project documentation
├── 📄 SYSTEM_OVERVIEW.md       # Complete system guide
├── 📄 .gitattributes           # Git configuration
│
├── 📂 docs/                    # All documentation (organized)
│   ├── 📄 DOCS_INDEX.md       # Documentation index
│   │
│   ├── 📂 architecture/       # Design & architecture docs
│   │   ├── ARCHITECTURE.md
│   │   ├── ARCHITECTURE_V3.md
│   │   └── ARCHITECTURE_VISUAL.md
│   │
│   ├── 📂 guides/             # User guides
│   │   ├── MODES_GUIDE.md
│   │   ├── DEVELOPMENT.md
│   │   ├── INSTALLATION.md
│   │   └── MIGRATION.md
│   │
│   ├── 📂 api/                # API documentation
│   │   ├── API_REFERENCE.md
│   │   ├── REACT_APP_API.md
│   │   └── QUICK_REFERENCE.md
│   │
│   ├── CHANGELOG.md
│   ├── IMPROVEMENTS_SUMMARY.md
│   └── TRANSFORMATION_SUMMARY.md
│
├── 📂 scripts/                # Executable scripts
│   ├── start-dev.sh
│   └── start-prod.sh
│
├── 📂 WallboxCtrl/            # Main C++ application
│   ├── include/              # Header files
│   │   ├── Configuration.h
│   │   ├── GpioFactory.h
│   │   ├── ApiController.h
│   │   ├── Application.h
│   │   └── ...
│   ├── src/                  # Source files
│   │   ├── main_v3.cpp
│   │   ├── WallboxController.cpp
│   │   └── ...
│   ├── build/                # Build artifacts
│   ├── tests/                # Unit tests
│   └── CMakeLists.txt
│
├── 📂 wallbox-react-app/     # React web interface
│   ├── src/
│   ├── public/
│   └── package.json
│
├── 📂 LibPubWallbox/         # Protocol library
└── 📂 env/                   # Docker environment
```

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
```

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
