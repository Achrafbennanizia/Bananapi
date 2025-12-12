#!/bin/bash
#
# Wallbox Controller - Installation Script
# Compatible with: Raspberry Pi, Banana Pi, Orange Pi, and other ARM SBCs
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
INSTALL_DIR="${HOME}/wallbox"
LOG_FILE="/tmp/wallbox_install.log"

# Functions
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
        else
            echo "Generic ARM"
        fi
    else
        echo "Unknown"
    fi
}

check_dependencies() {
    log "Checking dependencies..."
    
    MISSING_DEPS=()
    
    for cmd in gcc g++ cmake make git; do
        if ! command -v $cmd &> /dev/null; then
            MISSING_DEPS+=($cmd)
        fi
    done
    
    if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
        warn "Missing dependencies: ${MISSING_DEPS[*]}"
        return 1
    fi
    
    return 0
}

install_dependencies() {
    log "Installing dependencies..."
    
    sudo apt update || error "Failed to update package list"
    
    sudo apt install -y \
        build-essential \
        cmake \
        git \
        pkg-config \
        libmicrohttpd-dev \
        libcurl4-openssl-dev \
        || error "Failed to install dependencies"
    
    log "Dependencies installed successfully"
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
    log "Building project..."
    
    cd "$INSTALL_DIR"
    
    if [ -d "build" ]; then
        rm -rf build/*
    fi
    mkdir -p build
    cd build
    
    log "Running CMake..."
    cmake .. || error "CMake configuration failed"
    
    log "Compiling (this may take a few minutes)..."
    make -j$(nproc) || error "Compilation failed"
    
    # Copy config
    if [ ! -f config.json ] && [ -f ../config/config.json ]; then
        cp ../config/config.json .
    fi
    
    log "Build completed successfully"
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
Description=Wallbox Controller Service
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

[Install]
WantedBy=multi-user.target
EOF

    sudo cp /tmp/wallbox.service /etc/systemd/system/
    sudo systemctl daemon-reload
    
    log "Systemd service created (not enabled yet)"
}

print_summary() {
    echo ""
    echo "======================================"
    echo "  Installation Complete!"
    echo "======================================"
    echo ""
    echo "Installation directory: $INSTALL_DIR"
    echo "Executables: $INSTALL_DIR/build/"
    echo "Configuration: $INSTALL_DIR/build/config.json"
    echo ""
    echo "Next steps:"
    echo ""
    echo "1. Edit configuration:"
    echo "   nano $INSTALL_DIR/build/config.json"
    echo ""
    echo "2. Test run:"
    echo "   cd $INSTALL_DIR/build"
    echo "   ./wallbox_control_v3"
    echo ""
    echo "3. Enable service (optional):"
    echo "   sudo systemctl enable wallbox"
    echo "   sudo systemctl start wallbox"
    echo ""
    echo "4. Check status:"
    echo "   curl http://localhost:8080/api/status"
    echo ""
    echo "For more information, see:"
    echo "   $INSTALL_DIR/docs/INSTALLATION_GUIDE.md"
    echo ""
}

# Main installation process
main() {
    echo "======================================"
    echo "  Wallbox Controller Installation"
    echo "======================================"
    echo ""
    
    check_root
    
    PLATFORM=$(detect_platform)
    log "Platform detected: $PLATFORM"
    
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
    configure_firewall
    create_service
    
    print_summary
}

# Run installation
main "$@"
