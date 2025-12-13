#!/bin/bash
#
# Wallbox Controller - Deploy to Remote Pi
# Usage: ./deploy.sh <pi_hostname_or_ip> [ssh_user]
#
# Version: 4.1 (with CP Signal System)
# Date: December 13, 2025

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
PI_HOST="${1}"
SSH_USER="${PI_USER:-${2:-pi}}"
REMOTE_DIR="/home/$SSH_USER/wallbox-src"
LOCAL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"
BUILD_MODE="${BUILD_MODE:-production}"

# Functions
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
    if [ -z "$PI_HOST" ]; then
        echo "Usage: $0 <pi_hostname_or_ip> [ssh_user]"
        echo ""
        echo "Environment variables:"
        echo "  PI_USER=<user>       - SSH user (default: pi)"
        echo "  BUILD_MODE=<mode>    - production or development (default: production)"
        echo ""
        echo "Examples:"
        echo "  $0 192.168.178.34"
        echo "  PI_USER=root $0 192.168.178.34"
        echo "  BUILD_MODE=development $0 bananapi"
        exit 1
    fi
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
    log "Testing SSH connection to $SSH_USER@$PI_HOST..."
    
    if ! ssh -o ConnectTimeout=5 -o BatchMode=yes "$SSH_USER@$PI_HOST" "echo 'Connection OK'" &> /dev/null; then
        error "Cannot connect to $SSH_USER@$PI_HOST. Check:"
        echo "  - SSH is enabled on Pi"
        echo "  - Correct IP address/hostname"
        echo "  - SSH keys are set up"
        echo "  - User '$SSH_USER' exists"
    fi
    
    log "Connection successful"
    
    # Check sudo permissions
    log "Checking sudo permissions..."
    if ssh -o ConnectTimeout=5 "$SSH_USER@$PI_HOST" "sudo -n true" &> /dev/null; then
        log "Passwordless sudo is configured"
    else
        warn "Passwordless sudo not configured for user '$SSH_USER'"
        echo ""
        echo "Options to fix this:"
        echo "  1. Run with root user:"
        echo "     PI_USER=root $0 $PI_HOST"
        echo ""
        echo "  2. Configure passwordless sudo on Pi:"
        echo "     ssh $SSH_USER@$PI_HOST"
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
    log "Packaging project..."
    
    cd "$LOCAL_DIR"
    
    PACKAGE_NAME="wallbox-deploy-$(date +%Y%m%d_%H%M%S).tar.gz"
    
    # Package all source files including CP signal system
    tar -czf "/tmp/$PACKAGE_NAME" \
        --exclude='build' \
        --exclude='.git' \
        --exclude='*.o' \
        --exclude='*.log' \
        --exclude='__pycache__' \
        --exclude='.DS_Store' \
        src/ include/ config/ scripts/ \
        CMakeLists.txt Makefile \
        README.md QUICK_START.md BUILD_METHODS.md \
        docs/ 2>/dev/null || true
    
    if [ ! -f "/tmp/$PACKAGE_NAME" ]; then
        error "Failed to create package"
    fi
    
    SIZE=$(du -h "/tmp/$PACKAGE_NAME" | cut -f1)
    log "Package created: $PACKAGE_NAME ($SIZE)"
    
    echo "/tmp/$PACKAGE_NAME"
}

deploy_to_pi() {
    log "Deploying to $SSH_USER@$PI_HOST..."
    
    PACKAGE=$(package_project)
    
    # Create remote directory
    ssh "$SSH_USER@$PI_HOST" "mkdir -p $REMOTE_DIR" || error "Failed to create remote directory"
    
    # Copy package
    log "Copying files..."
    scp "$PACKAGE" "$SSH_USER@$PI_HOST:/tmp/" || error "Failed to copy package"
    
    # Extract on remote
    log "Extracting files on remote..."
    ssh "$SSH_USER@$PI_HOST" "cd $REMOTE_DIR && tar -xzf /tmp/$(basename $PACKAGE)" || error "Failed to extract package"
    
    # Cleanup
    rm "$PACKAGE"
    ssh "$SSH_USER@$PI_HOST" "rm /tmp/$(basename $PACKAGE)"
    
    log "Files deployed successfully"
}

install_remote_dependencies() {
    log "Checking and installing dependencies on Pi..."
    
    # Use -t flag to allocate pseudo-terminal for sudo
    ssh -t "$SSH_USER@$PI_HOST" "bash -s" << 'EOF'
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
    
    ssh "$SSH_USER@$PI_HOST" "cd $REMOTE_DIR && BUILD_MODE=$BUILD_MODE bash scripts/install.sh" || error "Build failed"
    
    log "Build completed on Pi"
}

configure_system() {
    log "Configuring system..."
    
    # Get local machine IP
    LOCAL_IP=$(ifconfig 2>/dev/null | grep "inet " | grep -v 127.0.0.1 | awk '{print $2}' | head -1)
    
    if [ -z "$LOCAL_IP" ]; then
        LOCAL_IP=$(ip addr 2>/dev/null | grep "inet " | grep -v 127.0.0.1 | awk '{print $2}' | cut -d'/' -f1 | head -1)
    fi
    
    log "Detected local IP: ${LOCAL_IP:-unknown}"
    
    # Update config.json on remote with proper mode
    ssh "$SSH_USER@$PI_HOST" "cd $REMOTE_DIR/build && \
        python3 -c \"
import json
with open('config.json', 'r') as f:
    config = json.load(f)
config['mode'] = '$BUILD_MODE'
if '$LOCAL_IP':
    config['network']['udp_send_address'] = '$LOCAL_IP'
with open('config.json', 'w') as f:
    json.dump(config, f, indent=2)
print('Configuration updated')
\" || sed -i 's/\"mode\": \".*\"/\"mode\": \"$BUILD_MODE\"/' config.json"
    
    log "System configured for $BUILD_MODE mode"
}

check_gpio_permissions() {
    log "Checking GPIO permissions..."
    
    if [ "$BUILD_MODE" = "production" ]; then
        ssh "$SSH_USER@$PI_HOST" "test -w /sys/class/gpio/export" && \
            log "GPIO permissions OK" || \
            warn "GPIO may not be accessible. Run with sudo or add user to gpio group"
    else
        log "Development mode - GPIO permissions not required"
    fi
}

start_service() {
    log "Starting wallbox service..."
    
    # Stop any existing instances
    ssh "$SSH_USER@$PI_HOST" "pkill -9 wallbox_control || true"
    sleep 1
    
    # Start service
    ssh "$SSH_USER@$PI_HOST" "cd $REMOTE_DIR/build && \
        nohup ./wallbox_control_v3 </dev/null >/tmp/wallbox.log 2>&1 &"
    
    sleep 3
    
    # Check if running
    if ssh "$SSH_USER@$PI_HOST" "pgrep wallbox_control" &> /dev/null; then
        PID=$(ssh "$SSH_USER@$PI_HOST" "pgrep wallbox_control")
        log "Wallbox service started (PID: $PID)"
    else
        warn "Wallbox may not have started. Check logs with:"
        echo "    ssh $SSH_USER@$PI_HOST 'tail -50 /tmp/wallbox.log'"
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
        ssh "$SSH_USER@$PI_HOST" "grep -i 'HardwareCpSignalReader' /tmp/wallbox.log" && \
            log "Hardware CP reader initialized" || \
            warn "CP reader may not be initialized"
    else
        log "Development mode - CP signal via simulator"
        ssh "$SSH_USER@$PI_HOST" "grep -i 'SimulatorCpSignalReader' /tmp/wallbox.log" && \
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
    echo "Remote: $SSH_USER@$PI_HOST"
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
    echo "    ssh $SSH_USER@$PI_HOST 'tail -f /tmp/wallbox.log'"
    echo ""
    echo "  Check process:"
    echo "    ssh $SSH_USER@$PI_HOST 'ps aux | grep wallbox_control'"
    echo ""
    echo "  Stop service:"
    echo "    ssh $SSH_USER@$PI_HOST 'pkill wallbox_control'"
    echo ""
    echo "  Restart service:"
    echo "    ssh $SSH_USER@$PI_HOST 'cd $REMOTE_DIR/build && nohup ./wallbox_control_v3 </dev/null >/tmp/wallbox.log 2>&1 &'"
    echo ""
    echo "  Enable systemd service:"
    echo "    ssh $SSH_USER@$PI_HOST 'sudo systemctl enable wallbox && sudo systemctl start wallbox'"
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
