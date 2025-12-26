# Build Methods Comparison

## Overview

The wallbox-portable-deploy package supports **two build systems**:

1. **Make** - Traditional, simple, fast
2. **CMake** - Modern, flexible, cross-platform

Both produce identical C++14 binaries.

---

## Quick Comparison

| Feature                | Make                        | CMake                                 |
| ---------------------- | --------------------------- | ------------------------------------- |
| **Setup**              | No configuration needed     | Requires `cmake ..` first             |
| **Speed**              | Faster (direct compilation) | Slightly slower (generates Makefiles) |
| **Simplicity**         | Very simple                 | More complex                          |
| **Dependencies**       | Manual in Makefile          | Auto-detection                        |
| **IDE Support**        | Limited                     | Excellent                             |
| **Deployment Scripts** | Not used by default         | Used by install.sh                    |
| **Best For**           | Quick builds, development   | Automation, CI/CD                     |

---

## Build with Make

### Simple Build

```bash
cd wallbox-portable-deploy
make
```

That's it! Binary will be in `build/wallbox_control_v3`

### Advanced Usage

```bash
# Check dependencies before building
make check-deps

# Build only wallbox (not simulator)
make wallbox

# Build only simulator
make simulator

# Clean and rebuild
make clean
make

# Install to system
sudo make install

# See all options
make help
```

### Makefile Structure

```makefile
# Compiler: g++
# Standard: C++14
# Flags: -std=c++14 -Wall -Wextra -O2 -pthread
# Libraries: libmicrohttpd, libcurl
```

### Output

```
build/
├── wallbox_control_v3    # Main binary
├── simulator             # Simulator binary
└── *.o                   # Object files
```

---

## Build with CMake

### Standard Build

```bash
cd wallbox-portable-deploy
mkdir -p build && cd build
cmake ..
make
```

### Advanced Usage

```bash
# Build with verbose output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
make VERBOSE=1

# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make

# Release build (optimized)
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Use ninja instead of make
cmake .. -G Ninja
ninja

# Parallel build (faster)
make -j$(nproc)

# Install
sudo make install
```

### CMake Structure

```cmake
# Minimum version: 3.10
# Standard: C++14
# Targets: wallbox_control_v3, simulator
# Dependencies: libmicrohttpd, libcurl, Threads
```

---

## Which One to Use?

### Use Make When:

- ✅ You want quick builds
- ✅ You're familiar with traditional Make
- ✅ You're doing rapid development/testing
- ✅ You don't need CMake features
- ✅ You want direct control

### Use CMake When:

- ✅ You're using the deployment scripts
- ✅ You need IDE integration (CLion, VS Code)
- ✅ You want better dependency detection
- ✅ You're setting up CI/CD
- ✅ You need cross-platform builds

---

## Example Workflows

### Quick Development (Make)

```bash
# Edit code
vim src/main_v3.cpp

# Build
make

# Test
cd build && ./wallbox_control_v3

# Make changes, rebuild
cd ..
make
```

### Full Development (CMake)

```bash
# Initial setup
mkdir build && cd build
cmake ..

# Build
make

# Make changes, rebuild
make

# Run
./wallbox_control_v3
```

### Deployment (CMake)

```bash
# Automated deployment uses CMake
./scripts/deploy.sh <API_HOST> root

# On the Pi, the script runs:
# cd ~/wallbox-control
# mkdir build && cd build
# cmake ..
# make
```

---

## Dependencies

Both build systems require the same dependencies:

```bash
# Debian/Ubuntu/Raspberry Pi OS
sudo apt install -y \
    build-essential \
    libmicrohttpd-dev \
    libcurl4-openssl-dev

# For CMake build, also install:
sudo apt install -y cmake
```

---

## Troubleshooting

### Make Build Issues

**Problem:** `make: *** [check-deps] Error 1`

```bash
# Install missing dependencies
sudo apt update
sudo apt install -y build-essential libmicrohttpd-dev libcurl4-openssl-dev
```

**Problem:** Compilation errors

```bash
# Clean and rebuild
make clean
make
```

**Problem:** `g++: command not found`

```bash
sudo apt install -y build-essential
```

### CMake Build Issues

**Problem:** `CMake not found`

```bash
sudo apt install -y cmake
```

**Problem:** `libmicrohttpd not found`

```bash
sudo apt install -y libmicrohttpd-dev
```

**Problem:** Out-of-date CMake cache

```bash
cd build
rm -rf *
cmake ..
make
```

---

## Performance Comparison

Tested on Raspberry Pi 3:

| Task                  | Make    | CMake                     |
| --------------------- | ------- | ------------------------- |
| **First Build**       | ~2 min  | ~2.5 min (includes cmake) |
| **Incremental Build** | ~5 sec  | ~5 sec                    |
| **Clean Build**       | ~2 min  | ~2 min                    |
| **Dependency Check**  | Instant | ~10 sec                   |

**Conclusion:** Make is slightly faster for initial builds, but both are equivalent for development.

---

## File Structure

Both systems use the same source files:

```
wallbox-portable-deploy/
├── Makefile              ← Make build system
├── CMakeLists.txt        ← CMake build system
├── src/                  ← Source files (shared)
├── include/              ← Headers (shared)
└── build/                ← Output (shared)
```

---

## Recommendation

**For most users:** Use **Make** for simplicity and speed.

**For deployment:** The scripts use **CMake** (already configured).

**For development:** Choose based on preference:

- **Make** = Simple, traditional
- **CMake** = Modern, feature-rich

Both work perfectly! 🎉

---

## Quick Commands Reference

### Make

```bash
make              # Build all
make wallbox      # Build wallbox only
make simulator    # Build simulator only
make clean        # Clean
make install      # Install
make help         # Show help
```

### CMake

```bash
mkdir build && cd build
cmake ..          # Configure
make              # Build
make clean        # Clean
make install      # Install
```

---

_Both build systems produce identical C++14 binaries ready for deployment._
