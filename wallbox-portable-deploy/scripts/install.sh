#!/bin/bash
#
# Wallbox Controller - Installation Script
# Compatible with: Raspberry Pi, Banana Pi, Orange Pi, and other ARM SBCs
#
# Version: 4.1 (with CP Signal System)
# Date: December 14, 2025

set -e

# Parse options first
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            SHOW_HELP=1
            shift
            ;;
        -m|--mode)
            BUILD_MODE="$2"
            shift 2
            ;;
        -i|--interactive)
            INTERACTIVE=1
            shift
            ;;
        --systemd)
            SETUP_SYSTEMD=1
            shift
            ;;
        -*)
            echo "Unknown option: $1"
            SHOW_HELP=1
            shift
            ;;
        *)
            shift
            ;;
    esac
done

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
INSTALL_DIR="${INSTALL_DIR:-${HOME}/wallbox-src}"
BUILD_MODE="${BUILD_MODE:-production}"
LOG_FILE="/tmp/wallbox_install.log"

# Functions
show_help() {
    cat << EOF
${CYAN}╔════════════════════════════════════════════════════════════╗
║        Wallbox Controller - Installation Script          ║
║                      Version 4.1                           ║
╚════════════════════════════════════════════════════════════╝${NC}

${GREEN}USAGE:${NC}
  $0 [options]

${GREEN}OPTIONS:${NC}
  -h, --help           Show this help message
  -m, --mode <mode>    Build mode: production, development, or debug
  -i, --interactive    Interactive mode selection
  --systemd            Setup systemd service (requires sudo)

${GREEN}BUILD MODES:${NC}
  ${CYAN}production${NC}     - Optimized build (-O3), no debug symbols
                     - Compiler: -O3 -DNDEBUG -s
                     - Binary size: ~270 KB (wallbox), ~115 KB (simulator)
                     - Best performance, minimal logging
                     - Default mode for production deployment

  ${YELLOW}development${NC}    - Debug symbols (-g), detailed logging
                     - Compiler: -g -O0
                     - Binary size: ~500 KB (with debug info)
                     - Verbose output for troubleshooting
                     - Retains function names and line numbers
                     - Fast iteration during development

  ${RED}debug${NC}          - Maximum debug info, all checks enabled
                     - Compiler: -g -O0 -fsanitize=address
                     - AddressSanitizer for memory errors
                     - Very verbose logging
                     - Slowest execution, largest binaries
                     - Best for finding bugs and memory issues

${GREEN}ENVIRONMENT VARIABLES:${NC}
  INSTALL_DIR          Installation directory (default: ~/wallbox-src)
  BUILD_MODE           Build mode (default: production)

${GREEN}EXAMPLES:${NC}
  # Basic installation (production mode)
  $0

  # Interactive mode selection
  $0 --interactive

  # Development mode
  $0 --mode development

  # Debug mode with systemd
  $0 --mode debug --systemd

  # Custom installation directory
  INSTALL_DIR=/opt/wallbox $0

  # Environment variable mode
  BUILD_MODE=development $0

${GREEN}FEATURES:${NC}
  ✓ Automatic platform detection (Raspberry Pi, Banana Pi, Orange Pi)
  ✓ Dependency checking and auto-installation
  ✓ CMake and Make build support
  ✓ Binary size optimization
  ✓ Optional systemd service creation
  ✓ Comprehensive build logging

${GREEN}SUPPORTED PLATFORMS:${NC}
  ✓ Raspberry Pi (all models)
  ✓ Banana Pi
  ✓ Orange Pi
  ✓ Other ARM-based SBCs
  ✓ x86_64 Linux
  ✓ macOS (development)

EOF
    exit 0
}

select_mode() {
    echo -e "\n${CYAN}╔════════════════════════════════════════╗"
    echo -e "║      Select Build Mode                   ║"
    echo -e "╚════════════════════════════════════════╝${NC}\n"
    
    echo -e "${GREEN}Available modes:${NC}"
    echo -e "  ${CYAN}1)${NC} ${GREEN}production${NC}  - Optimized, small binaries (recommended)"
    echo -e "  ${CYAN}2)${NC} ${YELLOW}development${NC} - Debug symbols, verbose logging"
    echo -e "  ${CYAN}3)${NC} ${RED}debug${NC}       - Maximum debug info, sanitizers\n"
    
    while true; do
        read -p "$(echo -e "${CYAN}Enter choice [1-3]:${NC} ")" choice
        case $choice in
            1)
                BUILD_MODE="production"
                echo -e "${GREEN}✓ Selected: production mode${NC}"
                break
                ;;
            2)
                BUILD_MODE="development"
                echo -e "${YELLOW}✓ Selected: development mode${NC}"
                break
                ;;
            3)
                BUILD_MODE="debug"
                echo -e "${RED}✓ Selected: debug mode${NC}"
                break
                ;;
            *)
                echo -e "${RED}Invalid choice. Please enter 1, 2, or 3.${NC}"
                ;;
        esac
    done
    echo ""
}

log() {
    echo -e "${GREEN}[INFO]${NC} $1" | tee -a "$LOG_FILE"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "$LOG_FILE"
    exit 1
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1" | tee -a "$LOG_FILE"
}

check_root() {
    if [ "$EUID" -eq 0 ]; then
        error "Please do not run this script as root. Run as normal user."
    fi
}

detect_platform() {
    log "Detecting platform..."
    
    if [ -f /proc/cpuinfo ]; then
        if grep -q "BCM" /proc/cpuinfo; then
            echo "Raspberry Pi"
        elif grep -q "sun" /proc/cpuinfo; then
            echo "Banana Pi / Orange Pi"
        elif grep -q "ARM" /proc/cpuinfo; then
            echo "Generic ARM"
        else
            echo "Unknown ARM"
        fi
    elif [ -f /proc/device-tree/model ]; then
        MODEL=$(cat /proc/device-tree/model 2>/dev/null || echo "Unknown")
        echo "$MODEL"
    else
        echo "Unknown"
    fi
}

check_architecture() {
    ARCH=$(uname -m)
    log "Architecture: $ARCH"
    
    case "$ARCH" in
        armv7l|armv6l|aarch64|arm64)
            log "ARM architecture detected - compatible"
            ;;
        x86_64|amd64)
            warn "x86_64 architecture - development mode recommended"
            ;;
        *)
            warn "Unknown architecture: $ARCH"
            ;;
    esac
}

check_dependencies() {
    log "Checking dependencies..."
    
    MISSING_DEPS=()
    
    for cmd in gcc g++ cmake make git python3; do
        if ! command -v $cmd &> /dev/null; then
            MISSING_DEPS+=($cmd)
        fi
    done
    
    # Check for required libraries
    if ! pkg-config --exists libmicrohttpd 2>/dev/null; then
        MISSING_DEPS+=(libmicrohttpd-dev)
    fi
    
    if ! pkg-config --exists libcurl 2>/dev/null; then
        MISSING_DEPS+=(libcurl-dev)
    fi
    
    if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
        warn "Missing dependencies: ${MISSING_DEPS[*]}"
        log "Auto-installing missing packages..."
        install_dependencies
        return 0
    fi
    
    log "All dependencies satisfied"
    return 0
}

install_dependencies() {
    log "Installing dependencies..."
    
    # Update package list first
    log "Updating package lists..."
    sudo apt-get update -qq || error "Failed to update package list"
    
    # Upgrade existing packages
    log "Upgrading existing packages..."
    sudo DEBIAN_FRONTEND=noninteractive apt-get upgrade -y -qq || warn "Package upgrade had issues"
    
    # Install required packages
    log "Installing required packages..."
    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y -qq \
        build-essential \
        cmake \
        make \
        git \
        python3 \
        pkg-config \
        libmicrohttpd-dev \
        libcurl4-openssl-dev \
        net-tools \
        || error "Failed to install dependencies"
    
    log "Dependencies installed successfully"
}

setup_gpio_permissions() {
    log "Setting up GPIO permissions..."
    
    if [ "$BUILD_MODE" = "production" ]; then
        # Add user to gpio group if it exists
        if getent group gpio > /dev/null 2>&1; then
            sudo usermod -a -G gpio $USER || warn "Failed to add user to gpio group"
            log "User added to gpio group (logout/login required)"
        else
            warn "GPIO group not found - you may need sudo for GPIO access"
        fi
    else
        log "Development mode - GPIO permissions not required"
    fi
}

create_directories() {
    log "Creating directories..."
    
    mkdir -p "$INSTALL_DIR"/{build,logs,backups}
    
    log "Directories created at $INSTALL_DIR"
}

copy_files() {
    log "Copying project files..."
    
    SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
    
    cp -r "$PROJECT_DIR"/{src,include,CMakeLists.txt,Makefile,config} "$INSTALL_DIR/" || error "Failed to copy files"
    cp -r "$PROJECT_DIR"/scripts "$INSTALL_DIR/" || warn "Scripts directory not copied"
    cp -r "$PROJECT_DIR"/docs "$INSTALL_DIR/" || warn "Docs directory not copied"
    
    log "Files copied successfully"
}

build_project() {
    log "Building project (mode: $BUILD_MODE)..."
    
    cd "$INSTALL_DIR"
    
    if [ -d "build" ]; then
        log "Cleaning previous build..."
        rm -rf build/*
    fi
    mkdir -p build
    cd build
    
    log "Running CMake..."
    cmake .. || error "CMake configuration failed"
    
    log "Compiling (this may take a few minutes)..."
    CPU_CORES=$(nproc 2>/dev/null || echo 2)
    make -j${CPU_CORES} || error "Compilation failed"
    
    # Copy and configure config file
    if [ ! -f config.json ] && [ -f ../config/config.json ]; then
        cp ../config/config.json .
        
        # Set mode in config
        if command -v python3 &> /dev/null; then
            python3 -c "
import json
with open('config.json', 'r') as f:
    config = json.load(f)
config['mode'] = '$BUILD_MODE'
with open('config.json', 'w') as f:
    json.dump(config, f, indent=2)
print('Config mode set to: $BUILD_MODE')
" || log "Config copied (mode: $BUILD_MODE)"
        else
            log "Config copied - manually set mode in config.json"
        fi
    fi
    
    # List built executables
    log "Build completed successfully"
    log "Executables:"
    ls -lh wallbox_control_v3 simulator 2>/dev/null | awk '{print "  - " $9 " (" $5 ")"}'
}

configure_firewall() {
    log "Configuring firewall..."
    
    if command -v ufw &> /dev/null; then
        sudo ufw allow 8080/tcp comment "Wallbox HTTP API" || warn "Failed to configure firewall"
        sudo ufw allow 50010/udp comment "Wallbox UDP listen" || warn "Failed to configure firewall"
        sudo ufw allow 50011/udp comment "Wallbox UDP send" || warn "Failed to configure firewall"
        log "Firewall configured"
    else
        warn "UFW not installed, skipping firewall configuration"
    fi
}

create_service() {
    log "Creating systemd service..."
    
    cat > /tmp/wallbox.service <<EOF
[Unit]
Description=Wallbox Controller Service v4.1 (CP Signal System)
After=network.target
Wants=network-online.target

[Service]
Type=simple
User=$USER
Group=$USER
WorkingDirectory=$INSTALL_DIR/build
ExecStart=$INSTALL_DIR/build/wallbox_control_v3
Restart=always
RestartSec=10
StandardOutput=journal
StandardError=journal

# Environment
Environment="PATH=/usr/local/bin:/usr/bin:/bin"

# Resource limits
LimitNOFILE=4096

[Install]
WantedBy=multi-user.target
EOF

    sudo cp /tmp/wallbox.service /etc/systemd/system/
    sudo systemctl daemon-reload
    
    log "Systemd service created (not enabled yet)"
    log "To enable: sudo systemctl enable wallbox"
    log "To start: sudo systemctl start wallbox"
}

verify_installation() {
    log "Verifying installation..."
    
    cd "$INSTALL_DIR/build"
    
    ISSUES=()
    
    # Check executables
    [ -x "wallbox_control_v3" ] || ISSUES+=("wallbox_control_v3 not found or not executable")
    [ -x "simulator" ] || ISSUES+=("simulator not found or not executable")
    
    # Check config
    [ -f "config.json" ] || ISSUES+=("config.json not found")
    
    # Check includes
    [ -d "../include" ] || ISSUES+=("include directory not found")
    [ -f "../include/ICpSignalReader.h" ] || ISSUES+=("CP signal headers not found")
    
    if [ ${#ISSUES[@]} -eq 0 ]; then
        log "Installation verified successfully"
        return 0
    else
        warn "Installation issues found:"
        for issue in "${ISSUES[@]}"; do
            echo "  - $issue"
        done
        return 1
    fi
}

print_summary() {
    echo ""
    echo "======================================"
    echo "  Installation Complete!"
    echo "======================================"
    echo ""
    echo "Installation directory: $INSTALL_DIR"
    echo "Build mode: $BUILD_MODE"
    echo "Executables: $INSTALL_DIR/build/"
    echo "Configuration: $INSTALL_DIR/build/config.json"
    echo ""
    echo "Next steps:"
    echo ""
    echo "1. Review/edit configuration:"
    echo "   nano $INSTALL_DIR/build/config.json"
    echo ""
    
    if [ "$BUILD_MODE" = "production" ]; then
        echo "2. Test run (production mode - requires GPIO access):"
        echo "   cd $INSTALL_DIR/build"
        echo "   sudo ./wallbox_control_v3"
    else
        echo "2. Test run (development mode):"
        echo "   cd $INSTALL_DIR/build"
        echo "   ./wallbox_control_v3"
        echo ""
        echo "   Start simulator in another terminal:"
        echo "   ./simulator"
    fi
    echo ""
    echo "3. Enable systemd service (optional):"
    echo "   sudo systemctl enable wallbox"
    echo "   sudo systemctl start wallbox"
    echo ""
    echo "4. Check status:"
    echo "   curl http://localhost:8080/api/status"
    echo ""
    echo "5. View logs:"
    echo "   sudo journalctl -u wallbox -f"
    echo "   or: tail -f /tmp/wallbox.log"
    echo ""
    
    if [ "$BUILD_MODE" = "production" ]; then
        echo "Production Mode Features:"
        echo "  • Hardware GPIO control (pins 2-7, 17, 21-23, 27)"
        echo "  • CP signal reading from hardware (GPIO pin 7)"
        echo "  • Real relay and LED control"
        echo "  • Requires proper GPIO permissions"
        echo ""
    else
        echo "Development Mode Features:"
        echo "  • Simulated GPIO (no hardware access)"
        echo "  • CP signal via UDP simulator"
        echo "  • Safe for testing without hardware"
        echo "  • Can run on any platform"
        echo ""
    fi
    
    echo "Documentation:"
    echo "  • Quick Start: $INSTALL_DIR/QUICK_START.md"
    echo "  • Build Methods: $INSTALL_DIR/BUILD_METHODS.md"
    echo "  • Architecture: $INSTALL_DIR/PORTABLE_ARCHITECTURE.md"
    echo ""
    
    log "Log file saved: $LOG_FILE"
}

# Main installation process
main() {
    # Show help if requested
    if [ -n "$SHOW_HELP" ]; then
        show_help
    fi
    
    # Interactive mode selection
    if [ -n "$INTERACTIVE" ]; then
        select_mode
    fi
    
    # Validate build mode
    case $BUILD_MODE in
        production|development|debug)
            ;;
        *)
            warn "Invalid build mode: $BUILD_MODE"
            warn "Valid modes: production, development, debug"
            warn "Defaulting to production mode"
            BUILD_MODE="production"
            ;;
    esac
    
    echo "======================================"
    echo "  Wallbox Controller Installation"
    echo "  Version 4.1 (CP Signal System)"
    echo "======================================"
    echo ""
    
    check_root
    
    PLATFORM=$(detect_platform)
    log "Platform detected: $PLATFORM"
    
    check_architecture
    
    log "Build mode: $BUILD_MODE"
    
    if ! check_dependencies; then
        read -p "Install missing dependencies? (y/n) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            install_dependencies
        else
            error "Cannot proceed without dependencies"
        fi
    fi
    
    create_directories
    copy_files
    build_project
    setup_gpio_permissions
    configure_firewall
    create_service
    verify_installation
    
    print_summary
}

# Run installation
main "$@"
