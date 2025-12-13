#!/bin/bash
# Start ISO 15118 Simulator
# Version: 3.0

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASE_DIR="$(dirname "$SCRIPT_DIR")"
BIN_DIR="$BASE_DIR/bin"

echo "🧪 Starting ISO 15118 Simulator"
echo "================================"

# Check if binary exists
if [ ! -f "$BIN_DIR/simulator" ]; then
    echo "❌ Error: simulator not found in $BIN_DIR"
    exit 1
fi

# Change to bin directory
cd "$BIN_DIR" || exit 1

# Start simulator
echo "📡 Starting simulator..."
echo "   UDP Listen: 50011"
echo "   UDP Send: 50010"
echo ""
echo "Available commands:"
echo "  help     - Show all commands"
echo "  status   - Show current status"
echo "  on/off   - Control contactor"
echo "  idle/ready/charge - Set charging state"
echo ""
echo "Press Ctrl+C to stop"
echo ""

./simulator
