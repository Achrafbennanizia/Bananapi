#!/bin/bash
# Transfer Wallbox to Banana Pi
# Usage: ./TRANSFER.sh <banana-pi-ip> [user]
# Or: ./TRANSFER.sh bananapi (uses SSH config)

if [ -z "$1" ]; then
    echo "Usage: $0 <banana-pi-ip> [user]"
    echo "Example: $0 192.168.1.100 pi"
    echo "Or: $0 bananapi  (uses SSH config with private key)"
    exit 1
fi

BANANA_PI_IP="$1"
USER="${2:-pi}"
REMOTE_DIR="~/wallbox"
SSH_KEY="$HOME/.ssh/bananapi_key"

# Use SSH config hostname if available (e.g., 'bananapi')
if [[ "$BANANA_PI_IP" == "bananapi"* ]]; then
    SSH_TARGET="$BANANA_PI_IP"
    SSH_OPTS="-F $HOME/.ssh/config"
else
    SSH_TARGET="$USER@$BANANA_PI_IP"
    if [ -f "$SSH_KEY" ]; then
        SSH_OPTS="-i $SSH_KEY"
        echo "🔑 Using private key: $SSH_KEY"
    else
        SSH_OPTS=""
    fi
fi

echo "📦 Transferring Wallbox System to Banana Pi"
echo "============================================="
echo "Target: $SSH_TARGET:$REMOTE_DIR"
echo ""

# Check if rsync is available
if command -v rsync &> /dev/null; then
    echo "Using rsync for fast transfer..."
    rsync -avz --progress \
        -e "ssh $SSH_OPTS" \
        --exclude '.git' \
        --exclude '*.o' \
        --exclude '*.a' \
        . "$SSH_TARGET:$REMOTE_DIR/"
else
    echo "Using scp for transfer..."
    scp $SSH_OPTS -r . "$SSH_TARGET:$REMOTE_DIR/"
fi

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Transfer complete!"
    echo ""
    echo "Next steps on Banana Pi:"
    echo "  ssh $SSH_TARGET"
    echo "  cd $REMOTE_DIR"
    echo "  chmod +x scripts/*.sh bin/*"
    echo "  ./scripts/start-wallbox.sh"
    echo ""
    echo "Or install as service:"
    echo "  sudo ./scripts/install-service.sh"
else
    echo ""
    echo "❌ Transfer failed"
    exit 1
fi
