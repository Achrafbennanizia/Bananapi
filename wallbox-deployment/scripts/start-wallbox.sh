#!/bin/bash
# Start Wallbox Control System
# Version: 3.0

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASE_DIR="$(dirname "$SCRIPT_DIR")"
BIN_DIR="$BASE_DIR/bin"
CONFIG_DIR="$BASE_DIR/config"

echo "🚀 Starting Wallbox Control System v3.0"
echo "========================================"

# Check if binary exists
if [ ! -f "$BIN_DIR/wallbox_control_v3" ]; then
    echo "❌ Error: wallbox_control_v3 not found in $BIN_DIR"
    exit 1
fi

# Check if config exists
if [ ! -f "$CONFIG_DIR/config.json" ]; then
    echo "⚠️  Warning: config.json not found, using defaults"
fi

# Change to bin directory
cd "$BIN_DIR" || exit 1

# Start wallbox
echo "📡 Starting wallbox controller..."
echo "   UDP Listen: 50010"
echo "   UDP Send: 50011"
echo "   Config: $CONFIG_DIR/config.json"
echo ""
echo "Press Ctrl+C to stop"
echo ""

./wallbox_control_v3
