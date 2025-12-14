#!/bin/bash

# Production mode startup script
# WARNING: This uses REAL GPIO pins on Banana Pi hardware!

echo "╔════════════════════════════════════════════════╗"
echo "║  Wallbox Controller - Production Mode          ║"
echo "║  ⚠️  WARNING: REAL HARDWARE CONTROL ⚠️          ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Check if running on Banana Pi
if [ ! -d "/sys/class/gpio" ]; then
    echo "❌ ERROR: GPIO sysfs not found!"
    echo "   This system does not appear to be a Banana Pi"
    echo "   or GPIO support is not enabled."
    exit 1
fi

# Set production mode
export WALLBOX_MODE=prod

# Check if build directory exists
if [ ! -d "WallboxCtrl/build" ]; then
    echo "❌ Build directory not found. Please run:"
    echo "   cd WallboxCtrl && mkdir -p build && cd build && cmake .. && make"
    exit 1
fi

# Check if executable exists
if [ ! -f "WallboxCtrl/build/wallbox_control_v4" ]; then
    echo "❌ wallbox_control_v4 not found. Building..."
    cd WallboxCtrl/build
    cmake .. && make wallbox_control_v4
    cd ../..
fi

echo "⚠️  PRODUCTION MODE CONFIRMATION"
echo ""
echo "This will control REAL hardware:"
echo "  • GPIO Pin 4  - Relay control"
echo "  • GPIO Pin 17 - Green LED"
echo "  • GPIO Pin 27 - Yellow LED"
echo "  • GPIO Pin 22 - Red LED"
echo "  • GPIO Pin 23 - Button input"
echo ""
read -p "Are you sure you want to continue? (yes/no): " confirm

if [ "$confirm" != "yes" ]; then
    echo "❌ Cancelled by user"
    exit 0
fi

echo ""
echo "🚀 Starting production environment..."
echo ""

# Function to cleanup on exit
cleanup() {
    echo ""
    echo "🛑 Shutting down..."
    kill $PID_API 2>/dev/null
    wait
    echo "✅ Cleanup complete"
    exit 0
}

trap cleanup INT TERM

# Start API server
echo "Starting API server with REAL GPIO (v4.0 architecture)..."
cd WallboxCtrl/build
./wallbox_control_v4 &
PID_API=$!
cd ../..

sleep 2

if ! ps -p $PID_API > /dev/null; then
    echo "❌ Failed to start API server"
    exit 1
fi

echo ""
echo "╔════════════════════════════════════════════════╗"
echo "║  Production Environment Running!               ║"
echo "╠════════════════════════════════════════════════╣"
echo "║                                                ║"
echo "║  ⚡ Using REAL Banana Pi GPIO pins             ║"
echo "║                                                ║"
echo "║  API Server: http://localhost:8080             ║"
echo "║                                                ║"
echo "║  You can control the wallbox via:              ║"
echo "║  • React App (if installed)                    ║"
echo "║  • REST API calls                              ║"
echo "║  • curl commands                               ║"
echo "║                                                ║"
echo "║  Press Ctrl+C to stop                          ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Wait for user interrupt
wait $PID_API
