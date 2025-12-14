# Project Restructure Plan - Industry Best Practices

## Current Structure Issues
- Root directory cluttered with multiple directories
- Inconsistent naming (WallboxCtrl vs wallbox-portable-deploy)
- Scripts scattered across root and subdirectories
- Documentation not well organized
- Build artifacts mixed with source

## Proposed Industry-Standard Structure

```
PJMT/
├── .github/                    # GitHub workflows, templates
├── build/                      # All build outputs (gitignored)
├── cmake/                      # CMake modules and scripts
├── docs/                       # All documentation
│   ├── api/                    # API documentation
│   ├── architecture/           # Architecture docs
│   ├── diagrams/               # UML and design diagrams
│   ├── guides/                 # User guides
│   └── doxygen/                # Generated API docs
├── examples/                   # Example code and configs
├── external/                   # Third-party dependencies
│   └── LibPubWallbox/          # ISO 15118 protocol library
├── include/                    # Public headers
│   └── wallbox/                # Namespace headers
├── scripts/                    # Build, deploy, utility scripts
│   ├── build/                  # Build scripts
│   ├── deploy/                 # Deployment scripts
│   └── test/                   # Test scripts
├── src/                        # Source code
│   ├── core/                   # Core wallbox logic
│   ├── gpio/                   # GPIO implementations
│   ├── network/                # Network communication
│   ├── signal/                 # CP Signal system
│   └── api/                    # HTTP API implementation
├── tests/                      # All tests
│   ├── unit/                   # Unit tests
│   ├── integration/            # Integration tests
│   └── fixtures/               # Test data
├── tools/                      # Development tools
│   ├── simulator/              # ISO 15118 simulator
│   └── cli/                    # Command-line tools
├── web/                        # Web interface
│   └── react-app/              # React dashboard
├── config/                     # Configuration files
│   ├── development.json
│   ├── production.json
│   └── test.json
├── .clang-format               # Code formatting
├── .editorconfig               # Editor config
├── .gitignore                  # Git ignore patterns
├── CMakeLists.txt              # Root CMake
├── Doxyfile                    # Doxygen config
├── LICENSE                     # License file
├── README.md                   # Project readme
├── CHANGELOG.md                # Version history
├── CONTRIBUTING.md             # Contribution guide
└── VERSION                     # Version file
```

## Migration Steps

1. Create new directory structure
2. Move source files to appropriate locations
3. Update CMakeLists.txt files
4. Update all include paths
5. Update documentation references
6. Run tests to verify
7. Update all .md files with new structure
8. Commit changes

## Benefits

✓ Clear separation of concerns
✓ Standard naming conventions
✓ Easy to navigate
✓ Scalable for future growth
✓ CI/CD friendly
✓ IDE friendly
✓ Follows C++ Core Guidelines
