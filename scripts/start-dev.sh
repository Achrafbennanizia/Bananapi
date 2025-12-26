#!/bin/bash

# Development mode startup script
# This script starts all necessary components for development mode

echo "╔════════════════════════════════════════════════╗"
echo "║  Wallbox Controller - Development Mode         ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Set development mode
export WALLBOX_MODE=dev

# Check if build directory exists
if [ ! -d "WallboxCtrl/build" ]; then
    echo "❌ Build directory not found. Please run:"
    echo "   cd WallboxCtrl && mkdir -p build && cd build && cmake .. && make"
    exit 1
fi

# Check if executables exist
if [ ! -f "WallboxCtrl/build/wallbox_control_v4" ]; then
    echo "❌ wallbox_control_v4 not found. Building..."
    cd WallboxCtrl/build
    cmake .. && make wallbox_control_v4
    cd ../..
fi

if [ ! -f "WallboxCtrl/build/simulator" ]; then
    echo "❌ simulator not found. Building..."
    cd WallboxCtrl/build
    cmake .. && make
    cd ../..
fi

# Function to cleanup on exit
cleanup() {
    echo ""
    echo "🛑 Shutting down all processes..."
    kill $PID_SIMULATOR 2>/dev/null
    kill $PID_API 2>/dev/null
    wait
    echo "✅ Cleanup complete"
    exit 0
}

trap cleanup INT TERM

echo "🚀 Starting development environment..."
echo ""

# Start simulator in background
echo "1️⃣  Starting simulator on UDP ports 50010/50011..."
cd WallboxCtrl/build
./simulator > /tmp/wallbox_simulator.log 2>&1 &
PID_SIMULATOR=$!
cd ../..
sleep 1

if ! ps -p $PID_SIMULATOR > /dev/null; then
    echo "❌ Failed to start simulator"
    exit 1
fi
echo "   ✓ Simulator running (PID: $PID_SIMULATOR)"
echo "   📝 Log: /tmp/wallbox_simulator.log"
echo ""

# Start API server in background
echo "2️⃣  Starting API server (v4.0 architecture) on port 8080..."
cd WallboxCtrl/build
./wallbox_control_v4 > /tmp/wallbox_api.log 2>&1 &
PID_API=$!
cd ../..
sleep 2

if ! ps -p $PID_API > /dev/null; then
    echo "❌ Failed to start API server"
    kill $PID_SIMULATOR 2>/dev/null
    exit 1
fi
echo "   ✓ API server running (PID: $PID_API)"
echo "   📝 Log: /tmp/wallbox_api.log"
echo ""

# Check if React app is installed
if [ -d "wallbox-react-app/node_modules" ]; then
    echo "3️⃣  React app ready"
    echo "   ℹ️  To start React app manually:"
    echo "      cd wallbox-react-app && npm start"
else
    echo "3️⃣  React app not installed"
    echo "   ⚠️  To install and run React app:"
    echo "      cd wallbox-react-app && npm install && npm start"
fi

echo ""
echo "╔════════════════════════════════════════════════╗"
echo "║  Development Environment Ready!                ║"
echo "╠════════════════════════════════════════════════╣"
echo "║                                                ║"
echo "║  Components Running:                           ║"
echo "║  ✓ Simulator (UDP simulator)                   ║"
echo "║  ✓ API Server (REST API on port 8080)         ║"
echo "║                                                ║"
echo "║  Next Steps:                                   ║"
echo "║  1. Start React app:                           ║"
echo "║     cd wallbox-react-app && npm start          ║"
echo "║                                                ║"
echo "║  2. Open browser:                              ║"
echo "║     http://localhost:3000                      ║"
echo "║                                                ║"
echo "║  3. Control wallbox from React UI              ║"
echo "║                                                ║"
echo "║  Logs:                                         ║"
echo "║  • Simulator: /tmp/wallbox_simulator.log       ║"
echo "║  • API: /tmp/wallbox_api.log                   ║"
echo "║                                                ║"
echo "║  Press Ctrl+C to stop all services             ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Wait for user interrupt
wait
