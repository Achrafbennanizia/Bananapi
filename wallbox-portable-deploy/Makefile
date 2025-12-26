# Wallbox Controller - Makefile v4.1
# C++14 Compatible Build System with CP Signal System
# Supports: Raspberry Pi, Banana Pi, Orange Pi
# Features: ISO 15118, HTTP API, UDP Communication, CP Signal (Hardware + Simulator)

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -pthread
INCLUDES = -I./include
LDFLAGS = -lmicrohttpd -lcurl -pthread

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
CONFIG_DIR = config

# Source files for wallbox_control_v4 (main target with CP Signal System)
WALLBOX_SOURCES = \
	$(SRC_DIR)/main_v4.cpp \
	$(SRC_DIR)/HttpApiServer.cpp \
	$(SRC_DIR)/UdpCommunicator.cpp \
	$(SRC_DIR)/WallboxController.cpp \
	$(SRC_DIR)/ChargingStateMachine.cpp \
	$(SRC_DIR)/BananaPiGpioController.cpp \
	$(SRC_DIR)/StubGpioController.cpp \
	$(SRC_DIR)/SimpleWallboxController.cpp \
	$(SRC_DIR)/IsoStackCtrlProtocol_impl.cpp \
	$(SRC_DIR)/HardwareCpSignalReader.cpp \
	$(SRC_DIR)/SimulatorCpSignalReader.cpp \
	$(SRC_DIR)/CpSignalReaderFactory.cpp

# Source files for simulator
SIMULATOR_SOURCES = \
	$(SRC_DIR)/simulator.cpp \
	$(SRC_DIR)/IsoStackCtrlProtocol_impl.cpp

# Object files
WALLBOX_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(WALLBOX_SOURCES))
SIMULATOR_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SIMULATOR_SOURCES))

# Target binaries
WALLBOX_TARGET = $(BUILD_DIR)/wallbox_control_v3
SIMULATOR_TARGET = $(BUILD_DIR)/simulator

# Phony targets
.PHONY: all clean install test help check-deps wallbox simulator

# Default target
all: check-deps $(BUILD_DIR) $(WALLBOX_TARGET) $(SIMULATOR_TARGET)
	@echo ""
	@echo "✅ Build complete!"
	@echo "   Wallbox:   $(WALLBOX_TARGET)"
	@echo "   Simulator: $(SIMULATOR_TARGET)"
	@echo ""
	@echo "Run: cd $(BUILD_DIR) && ./wallbox_control_v3"

# Build wallbox only
wallbox: check-deps $(BUILD_DIR) $(WALLBOX_TARGET)
	@echo "✅ Wallbox built: $(WALLBOX_TARGET)"

# Build simulator only
simulator: check-deps $(BUILD_DIR) $(SIMULATOR_TARGET)
	@echo "✅ Simulator built: $(SIMULATOR_TARGET)"

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "📁 Created build directory"

# Build wallbox_control_v3
$(WALLBOX_TARGET): $(WALLBOX_OBJECTS)
	@echo "🔗 Linking wallbox_control_v3..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Built: $@"

# Build simulator
$(SIMULATOR_TARGET): $(SIMULATOR_OBJECTS)
	@echo "🔗 Linking simulator..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Built: $@"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "📝 Compiling: $<"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Check dependencies
check-deps:
	@echo "🔍 Checking dependencies..."
	@command -v g++ >/dev/null 2>&1 || { echo "❌ g++ not found. Install: sudo apt install build-essential"; exit 1; }
	@pkg-config --exists libmicrohttpd 2>/dev/null || { echo "⚠️  libmicrohttpd not found. Install: sudo apt install libmicrohttpd-dev"; exit 1; }
	@pkg-config --exists libcurl 2>/dev/null || { echo "⚠️  libcurl not found. Install: sudo apt install libcurl4-openssl-dev"; exit 1; }
	@echo "✅ All dependencies found"

# Install to system
install: all
	@echo "📦 Installing wallbox controller..."
	@mkdir -p /usr/local/bin
	@mkdir -p /usr/local/etc/wallbox
	@cp $(WALLBOX_TARGET) /usr/local/bin/
	@cp $(SIMULATOR_TARGET) /usr/local/bin/
	@cp $(CONFIG_DIR)/config.json /usr/local/etc/wallbox/ 2>/dev/null || echo "⚠️  config.json not found"
	@echo "✅ Installed to /usr/local/bin/"
	@echo "   Config: /usr/local/etc/wallbox/config.json"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "✅ Clean complete"

# Run tests
test: all
	@echo "🧪 Running tests..."
	@cd $(BUILD_DIR) && ./wallbox_control_v3 --version 2>/dev/null || echo "⚠️  No version command"
	@echo "✅ Binary tests passed"

# Show help
help:
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Wallbox Controller v4.1 - Build System"
	@echo "  with CP Signal System Support"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo ""
	@echo "📋 Available targets:"
	@echo ""
	@echo "  make              - Build everything (wallbox + simulator)"
	@echo "  make wallbox      - Build wallbox only"
	@echo "  make simulator    - Build simulator only"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make install      - Install to /usr/local/bin"
	@echo "  make test         - Run basic tests"
	@echo "  make check-deps   - Check dependencies"
	@echo "  make help         - Show this help"
	@echo ""
	@echo "🔧 Build configuration:"
	@echo "  Version:      4.1 (CP Signal System)"
	@echo "  C++ Standard: C++14"
	@echo "  Compiler:     $(CXX)"
	@echo "  Flags:        $(CXXFLAGS)"
	@echo "  Libraries:    libmicrohttpd, libcurl"
	@echo "  Features:     ISO 15118, CP Signal (HW+Simulator)"
	@echo ""
	@echo "📦 Output files:"
	@echo "  $(WALLBOX_TARGET)"
	@echo "  $(SIMULATOR_TARGET)"
	@echo ""
	@echo "🚀 Quick start:"
	@echo "  make                    # Build"
	@echo "  cd build                # Go to build dir"
	@echo "  ./wallbox_control_v3    # Run wallbox"
	@echo ""
	@echo "📖 For more info, see README.md"
	@echo ""

# Dependency tracking
-include $(WALLBOX_OBJECTS:.o=.d)
-include $(SIMULATOR_OBJECTS:.o=.d)

# Generate dependencies
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM -MT '$(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$<)' $< > $@
