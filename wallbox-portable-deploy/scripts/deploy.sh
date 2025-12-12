#!/bin/bash
#
# Wallbox Controller - Deploy to Remote Pi
# Usage: ./deploy.sh <pi_hostname_or_ip> [ssh_user]
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Configuration
PI_HOST="${1}"
SSH_USER="${2:-pi}"
REMOTE_DIR="/home/$SSH_USER/wallbox"
LOCAL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

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
        echo "Examples:"
        echo "  $0 192.168.178.34"
        echo "  $0 192.168.178.34 root"
        echo "  $0 bananapi pi"
        exit 1
    fi
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
}

package_project() {
    log "Packaging project..."
    
    cd "$LOCAL_DIR"
    
    PACKAGE_NAME="wallbox-deploy-$(date +%Y%m%d_%H%M%S).tar.gz"
    
    tar -czf "/tmp/$PACKAGE_NAME" \
        --exclude='build' \
        --exclude='.git' \
        --exclude='*.o' \
        --exclude='*.log' \
        src/ include/ config/ scripts/ CMakeLists.txt Makefile README.md 2>/dev/null || true
    
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

build_on_pi() {
    log "Building on Pi..."
    
    ssh "$SSH_USER@$PI_HOST" "cd $REMOTE_DIR && bash scripts/install.sh" || error "Build failed"
    
    log "Build completed on Pi"
}

configure_network() {
    log "Configuring network settings..."
    
    # Get local machine IP
    LOCAL_IP=$(ifconfig | grep "inet " | grep -v 127.0.0.1 | awk '{print $2}' | head -1)
    
    log "Detected local IP: $LOCAL_IP"
    
    # Update config.json on remote
    ssh "$SSH_USER@$PI_HOST" "cd $REMOTE_DIR/build && \
        sed -i 's/\"udp_send_address\": \".*\"/\"udp_send_address\": \"$LOCAL_IP\"/' config.json"
    
    log "Network configured"
}

start_service() {
    log "Starting wallbox service..."
    
    ssh "$SSH_USER@$PI_HOST" "cd $REMOTE_DIR/build && \
        pkill -9 wallbox_control || true && \
        ./wallbox_control_v3 </dev/null >/tmp/wallbox.log 2>&1 &"
    
    sleep 2
    
    # Check if running
    if ssh "$SSH_USER@$PI_HOST" "pgrep wallbox_control" &> /dev/null; then
        log "Wallbox service started successfully"
    else
        warn "Wallbox may not have started. Check logs with:"
        echo "    ssh $SSH_USER@$PI_HOST 'cat /tmp/wallbox.log'"
    fi
}

test_api() {
    log "Testing HTTP API..."
    
    sleep 2
    
    RESPONSE=$(curl -s -m 5 "http://$PI_HOST:8080/api/status" || echo "FAILED")
    
    if [[ "$RESPONSE" == *"state"* ]]; then
        log "API test successful!"
        echo "Response: $RESPONSE"
    else
        warn "API test failed. Service may still be starting..."
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
    echo ""
    echo "Useful commands:"
    echo ""
    echo "  Check status:"
    echo "    curl http://$PI_HOST:8080/api/status"
    echo ""
    echo "  View logs:"
    echo "    ssh $SSH_USER@$PI_HOST 'tail -f /tmp/wallbox.log'"
    echo ""
    echo "  Stop service:"
    echo "    ssh $SSH_USER@$PI_HOST 'pkill wallbox_control'"
    echo ""
    echo "  Restart service:"
    echo "    ssh $SSH_USER@$PI_HOST 'cd $REMOTE_DIR/build && ./wallbox_control_v3 </dev/null >/tmp/wallbox.log 2>&1 &'"
    echo ""
}

# Main deployment process
main() {
    echo "======================================"
    echo "  Wallbox Controller Deployment"
    echo "======================================"
    echo ""
    
    check_args
    test_connection
    deploy_to_pi
    configure_network
    start_service
    test_api
    
    print_summary
}

# Run deployment
main "$@"
