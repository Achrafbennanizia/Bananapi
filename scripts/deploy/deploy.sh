#!/bin/bash
#
# Wallbox Controller - Deploy to Remote Pi
# Usage: ./deploy.sh <pi_hostname_or_ip> [ssh_user] [options]
#
# Version: 4.1 (with CP Signal System)
# Date: December 14, 2025

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

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
        -*)
            echo "Unknown option: $1"
            SHOW_HELP=1
            shift
            ;;
        *)
            if [ -z "$PI_HOST" ]; then
                PI_HOST="$1"
            elif [ -z "$SSH_USER_ARG" ]; then
                SSH_USER_ARG="$1"
            fi
            shift
            ;;
    esac
done

# Configuration
SSH_USER="${PI_USER:-${SSH_USER_ARG:-pi}}"
# Handle root user directory differently
if [ "$SSH_USER" = "root" ]; then
    REMOTE_DIR="/root/wallbox-src"
else
    REMOTE_DIR="/home/$SSH_USER/wallbox-src"
fi
LOCAL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../.." && pwd )"
BUILD_MODE="${BUILD_MODE:-production}"

# Set SSH_TARGET based on whether PI_HOST is an SSH config alias
set_ssh_target() {
    if [ -n "$PI_HOST" ]; then
        if grep -q "^Host $PI_HOST$" ~/.ssh/config 2>/dev/null || grep -q "^Host $PI_HOST " ~/.ssh/config 2>/dev/null; then
            SSH_TARGET="$PI_HOST"
        else
            SSH_TARGET="$SSH_USER@$PI_HOST"
        fi
    fi
}

# Functions
show_help() {
    cat << EOF
${CYAN}╔════════════════════════════════════════════════════════════╗
║          Wallbox Controller - Deployment Script            ║
║                      Version 4.1                           ║
╚════════════════════════════════════════════════════════════╝${NC}

${GREEN}USAGE:${NC}
  $0 <pi_hostname_or_ip> [ssh_user] [options]

${GREEN}ARGUMENTS:${NC}
  pi_hostname_or_ip    Target Raspberry Pi / Banana Pi IP or hostname
  ssh_user             SSH user (default: pi)

${GREEN}OPTIONS:${NC}
  -h, --help           Show this help message
  -m, --mode <mode>    Deployment mode: production, development, or debug
  -i, --interactive    Interactive mode selection

${GREEN}DEPLOYMENT MODES:${NC}
  ${CYAN}production${NC}     - Optimized build (-O3), no debug symbols
                     - Small binaries (~270 KB wallbox, ~115 KB simulator)
                     - Best performance, minimal logging
                     - Recommended for production use

  ${YELLOW}development${NC}    - Debug symbols (-g), detailed logging
                     - Larger binaries with debug info
                     - Verbose output for troubleshooting
                     - Fast iteration during development

  ${RED}debug${NC}          - Maximum debug info, assertions enabled
                     - AddressSanitizer for memory debugging
                     - Very verbose logging
                     - Slowest, largest binaries

${GREEN}ENVIRONMENT VARIABLES:${NC}
  PI_USER              SSH user (overrides default)
  BUILD_MODE           Deployment mode (overrides default)
  REMOTE_DIR           Remote installation directory

${GREEN}EXAMPLES:${NC}
  # Basic deployment (production mode)
  $0 192.168.178.34

  # Interactive mode selection
  $0 192.168.178.34 --interactive

  # Development mode
  $0 192.168.178.34 --mode development

  # Custom user and debug mode
  PI_USER=root $0 bananapi.local --mode debug

  # Environment variable mode
  BUILD_MODE=development PI_USER=admin $0 192.168.178.34

${GREEN}FEATURES:${NC}
  ✓ Automatic dependency installation
  ✓ Remote compilation with optimization
  ✓ UDP network auto-configuration
  ✓ Optional systemd service setup
  ✓ Secure SSH-based deployment

EOF
    exit 0
}

select_mode() {
    echo -e "\n${CYAN}╔════════════════════════════════════════╗"
    echo -e "║    Select Deployment Mode              ║"
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
    echo -e "${GREEN}[INFO]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

check_args() {
    # Show help if requested
    if [ -n "$SHOW_HELP" ]; then
        show_help
    fi
    
    # Check required arguments
    if [ -z "$PI_HOST" ]; then
        echo -e "${RED}Error: Missing required argument${NC}"
        echo ""
        echo "Usage: $0 <pi_hostname_or_ip> [ssh_user] [options]"
        echo "Try '$0 --help' for more information."
        exit 1
    fi
    
    # Set SSH target (handles SSH config aliases)
    set_ssh_target
    
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
}

check_requirements() {
    log "Checking local requirements..."
    
    for cmd in ssh scp tar; do
        if ! command -v $cmd &> /dev/null; then
            error "Required command not found: $cmd"
        fi
    done
    
    log "Requirements satisfied"
}

test_connection() {
    log "Testing SSH connection to $SSH_TARGET..."
    
    if ! ssh -o ConnectTimeout=5 -o BatchMode=yes "$SSH_TARGET" "echo 'Connection OK'" &> /dev/null; then
        error "Cannot connect to $SSH_TARGET. Check:"
        echo "  - SSH is enabled on Pi"
        echo "  - Correct IP address/hostname"
        echo "  - SSH keys are set up"
        echo "  - User '$SSH_USER' exists"
    fi
    
    log "Connection successful"
    
    # Check sudo permissions
    log "Checking sudo permissions..."
    if ssh -o ConnectTimeout=5 "$SSH_TARGET" "sudo -n true" &> /dev/null; then
        log "Passwordless sudo is configured"
    else
        warn "Passwordless sudo not configured for user '$SSH_USER'"
        echo ""
        echo "Options to fix this:"
        echo "  1. Run with root user:"
        echo "     PI_USER=root $0 $PI_HOST"
        echo ""
        echo "  2. Configure passwordless sudo on Pi:"
        echo "     ssh $SSH_TARGET"
        echo "     echo \"$SSH_USER ALL=(ALL) NOPASSWD: ALL\" | sudo tee /etc/sudoers.d/$SSH_USER"
        echo ""
        echo "  3. Enter sudo password when prompted during deployment"
        echo ""
        read -p "Continue anyway? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            error "Deployment cancelled by user"
        fi
    fi
}

package_project() {
    log "Packaging project..." >&2
    
    cd "$LOCAL_DIR"
    
    PACKAGE_NAME="wallbox-deploy-$(date +%Y%m%d_%H%M%S).tar.gz"
    
    # Package all source files including CP signal system
    # Exclude macOS AppleDouble files (._*) which cause build errors on Linux
    COPYFILE_DISABLE=1 tar -czf "/tmp/$PACKAGE_NAME" \
        --exclude='build' \
        --exclude='.git' \
        --exclude='*.o' \
        --exclude='*.log' \
        --exclude='__pycache__' \
        --exclude='.DS_Store' \
        --exclude='._*' \
        src/ include/ external/ scripts/ \
        CMakeLists.txt Makefile config.json \
        README.md README_V2.md README_V4.md QUICK_START.md \
        CONFIG_GUIDE.md UDP_CONFIG_GUIDE.md V4_COMMANDS.md \
        docs/ 2>/dev/null || true
    
    if [ ! -f "/tmp/$PACKAGE_NAME" ]; then
        error "Failed to create package"
    fi
    
    SIZE=$(du -h "/tmp/$PACKAGE_NAME" | cut -f1)
    log "Package created: $PACKAGE_NAME ($SIZE)" >&2
    
    echo "/tmp/$PACKAGE_NAME"
}

deploy_to_pi() {
    log "Deploying to $SSH_TARGET..."
    
    PACKAGE=$(package_project)
    
    # Create remote directory
    ssh "$SSH_TARGET" "mkdir -p $REMOTE_DIR" || error "Failed to create remote directory"
    
    # Copy package using rsync (more reliable with SSH config)
    log "Copying files..."
    if command -v rsync &> /dev/null; then
        rsync -avz -e ssh "$PACKAGE" "$SSH_TARGET:/tmp/" || error "Failed to copy package"
    else
        # Fallback to scp with explicit SSH config
        scp -F ~/.ssh/config "$PACKAGE" "$SSH_TARGET:/tmp/" || error "Failed to copy package"
    fi
    
    # Extract on remote
    log "Extracting files on remote..."
    ssh "$SSH_TARGET" "cd $REMOTE_DIR && tar -xzf /tmp/$(basename $PACKAGE)" || error "Failed to extract package"
    
    # Cleanup
    rm "$PACKAGE"
    ssh "$SSH_TARGET" "rm /tmp/$(basename $PACKAGE)"
    
    log "Files deployed successfully"
}

install_remote_dependencies() {
    log "Checking and installing dependencies on Pi..."
    
    # Use -t flag to allocate pseudo-terminal for sudo
    ssh -t "$SSH_TARGET" "bash -s" << 'EOF'
        set -e
        echo "Updating package lists..."
        sudo apt-get update -qq
        
        echo "Upgrading packages..."
        sudo DEBIAN_FRONTEND=noninteractive apt-get upgrade -y -qq || echo "Warning: Upgrade had issues"
        
        echo "Installing dependencies..."
        sudo DEBIAN_FRONTEND=noninteractive apt-get install -y -qq \
            build-essential cmake make git python3 pkg-config \
            libmicrohttpd-dev libcurl4-openssl-dev net-tools
        
        echo "Dependencies installed successfully"
EOF
    
    if [ $? -eq 0 ]; then
        log "Remote dependencies satisfied"
    else
        error "Failed to install dependencies on Pi. You may need to configure passwordless sudo or run with PI_USER=root"
    fi
}

build_on_pi() {
    log "Building on Pi (mode: $BUILD_MODE)..."
    
    # Determine build type based on mode
    local CMAKE_BUILD_TYPE="Release"
    local EXTRA_FLAGS=""
    case "$BUILD_MODE" in
        development)
            CMAKE_BUILD_TYPE="Debug"
            EXTRA_FLAGS="-DENABLE_VERBOSE_LOGGING=ON"
            ;;
        debug)
            CMAKE_BUILD_TYPE="Debug"
            EXTRA_FLAGS="-DENABLE_SANITIZER=ON"
            ;;
        production)
            CMAKE_BUILD_TYPE="Release"
            ;;
    esac
    
    # Build directly using cmake
    ssh "$SSH_TARGET" "cd $REMOTE_DIR && \
        mkdir -p build && \
        cd build && \
        cmake .. -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE $EXTRA_FLAGS && \
        make -j\$(nproc)" || error "Build failed"
    
    log "Build completed on Pi"
}

configure_system() {
    log "Configuring system..."
    
    # Get local machine IP (for simulator -> Pi communication)
    LOCAL_IP=$(ifconfig 2>/dev/null | grep "inet " | grep -v 127.0.0.1 | awk '{print $2}' | head -1)
    
    if [ -z "$LOCAL_IP" ]; then
        LOCAL_IP=$(ip addr 2>/dev/null | grep "inet " | grep -v 127.0.0.1 | awk '{print $2}' | cut -d'/' -f1 | head -1)
    fi
    
    log "Detected local machine IP: ${LOCAL_IP:-unknown}"
    log "Pi IP: $PI_HOST"
    
    # Update config.json on remote with proper mode and IPs
    ssh "$SSH_TARGET" "cd $REMOTE_DIR/build && \
        python3 -c \"
import json
with open('config.json', 'r') as f:
    config = json.load(f)
config['mode'] = '$BUILD_MODE'
if '$LOCAL_IP':
    # Pi sends UDP responses back to the simulator (Mac)
    config['network']['udp_send_address'] = '$LOCAL_IP'
with open('config.json', 'w') as f:
    json.dump(config, f, indent=2)
print('Configuration updated')
print('  Mode: $BUILD_MODE')
print('  Pi will send UDP to: $LOCAL_IP')
\" || sed -i 's/\"mode\": \".*\"/\"mode\": \"$BUILD_MODE\"/' config.json"
    
    log "System configured for $BUILD_MODE mode"
    log "Pi will send UDP responses to: ${LOCAL_IP}"
}

check_gpio_permissions() {
    log "Checking GPIO permissions..."
    
    if [ "$BUILD_MODE" = "production" ]; then
        ssh "$SSH_TARGET" "test -w /sys/class/gpio/export" && \
            log "GPIO permissions OK" || \
            warn "GPIO may not be accessible. Run with sudo or add user to gpio group"
    else
        log "Development mode - GPIO permissions not required"
    fi
}

start_service() {
    log "Starting wallbox service..."
    
    # Stop any existing instances
    ssh "$SSH_TARGET" "pkill -9 wallbox_control || true"
    sleep 1
    
    # Start service
    ssh "$SSH_TARGET" "cd $REMOTE_DIR/build && \
        nohup ./wallbox_control_v4 </dev/null >/tmp/wallbox.log 2>&1 &"
    
    sleep 3
    
    # Check if running
    if ssh "$SSH_TARGET" "pgrep wallbox_control" &> /dev/null; then
        PID=$(ssh "$SSH_TARGET" "pgrep wallbox_control")
        log "Wallbox service started (PID: $PID)"
    else
        warn "Wallbox may not have started. Check logs with:"
        echo "    ssh $SSH_TARGET 'tail -50 /tmp/wallbox.log'"
        return 1
    fi
}

test_api() {
    log "Testing HTTP API..."
    
    sleep 2
    
    # Test health endpoint
    RESPONSE=$(curl -s -m 10 "http://$PI_HOST:8080/health" 2>/dev/null || echo "")
    
    if [[ "$RESPONSE" == *"status"* ]]; then
        log "API health check successful!"
        
        # Get full status
        STATUS=$(curl -s -m 5 "http://$PI_HOST:8080/api/status" 2>/dev/null || echo "")
        if [[ "$STATUS" == *"state"* ]]; then
            echo -e "${BLUE}Status:${NC} $STATUS"
        fi
    else
        warn "API test failed. Service may still be starting..."
        warn "Manual test: curl http://$PI_HOST:8080/api/status"
    fi
}

test_cp_signal() {
    log "Testing CP signal system..."
    
    if [ "$BUILD_MODE" = "production" ]; then
        log "Production mode - CP signal reading from hardware GPIO pin"
        ssh "$SSH_TARGET" "grep -i 'HardwareCpSignalReader' /tmp/wallbox.log" && \
            log "Hardware CP reader initialized" || \
            warn "CP reader may not be initialized"
    else
        log "Development mode - CP signal via simulator"
        ssh "$SSH_TARGET" "grep -i 'SimulatorCpSignalReader' /tmp/wallbox.log" && \
            log "Simulator CP reader initialized" || \
            warn "CP reader may not be initialized"
    fi
}

print_summary() {
    echo ""
    echo "======================================"
    echo "  Deployment Complete!"
    echo "======================================"
    echo ""
    echo "Remote: $SSH_TARGET"
    echo "Directory: $REMOTE_DIR"
    echo "Mode: $BUILD_MODE"
    echo ""
    echo "Services:"
    echo "  • HTTP API: http://$PI_HOST:8080"
    echo "  • UDP Ports: 50010 (RX), 50011 (TX)"
    echo "  • CP Signal: $([ "$BUILD_MODE" = "production" ] && echo "Hardware GPIO" || echo "UDP Simulator")"
    echo ""
    echo "Useful commands:"
    echo ""
    echo "  Check status:"
    echo "    curl http://$PI_HOST:8080/api/status"
    echo ""
    echo "  View logs:"
    echo "    ssh $SSH_TARGET 'tail -f /tmp/wallbox.log'"
    echo ""
    echo "  Check process:"
    echo "    ssh $SSH_TARGET 'ps aux | grep wallbox_control'"
    echo ""
    echo "  Stop service:"
    echo "    ssh $SSH_TARGET 'pkill wallbox_control'"
    echo ""
    echo "  Restart service:"
    echo "    ssh $SSH_TARGET 'cd $REMOTE_DIR/build && nohup ./wallbox_control_v4 </dev/null >/tmp/wallbox.log 2>&1 &'"
    echo ""
    echo "  Enable systemd service:"
    echo "    ssh $SSH_TARGET 'sudo systemctl enable wallbox && sudo systemctl start wallbox'"
    echo ""
    
    if [ "$BUILD_MODE" = "development" ]; then
        echo "Development Mode Notes:"
        echo "  • Run simulator locally: cd WallboxCtrl/build && ./simulator"
        echo "  • Simulator sends CP states via UDP to port 50010"
        echo "  • GPIO operations are simulated (no hardware access)"
        echo ""
    fi
}

# Main deployment process
main() {
    echo "======================================"
    echo "  Wallbox Controller Deployment"
    echo "  Version 4.1 (CP Signal System)"
    echo "======================================"
    echo ""
    
    check_args
    check_requirements
    test_connection
    install_remote_dependencies
    deploy_to_pi
    build_on_pi
    configure_system
    check_gpio_permissions
    start_service
    test_api
    test_cp_signal
    
    print_summary
}

# Run deployment
main "$@"
