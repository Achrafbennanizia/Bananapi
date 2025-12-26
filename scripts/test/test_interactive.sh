#!/bin/bash

echo "=== Wallbox Control System - Interactive Test ==="
echo ""
echo "This demonstrates the bidirectional communication:"
echo "- Both programs accept terminal commands"
echo "- Both programs communicate via UDP"
echo "- Changes are only logged when state changes"
echo ""

cd /Users/achraf/pro/PJMT/WallboxCtrl/src

echo "Starting wallbox_ctrl in background..."
./wallbox_ctrl > /tmp/wallbox.log 2>&1 &
WALLBOX_PID=$!
sleep 2

echo "Starting simulator in background..."
./simulator > /tmp/simulator.log 2>&1 &
SIM_PID=$!
sleep 2

echo ""
echo "=== System Running ==="
echo "Wallbox PID: $WALLBOX_PID"
echo "Simulator PID: $SIM_PID"
echo ""

echo "Checking initial logs..."
echo "--- Wallbox log (last 5 lines) ---"
tail -5 /tmp/wallbox.log
echo ""
echo "--- Simulator log (last 5 lines) ---"
tail -5 /tmp/simulator.log
echo ""

echo "=== To interact with the programs ==="
echo ""
echo "Terminal 1 - Wallbox Control:"
echo "  ./wallbox_ctrl"
echo "  Commands: enable, disable, status, help, quit"
echo ""
echo "Terminal 2 - Simulator:"
echo "  ./simulator"
echo "  Commands: on, off, idle, ready, charge, stop, status, help, quit"
echo ""
echo "=== Test Sequence ==="
echo "1. In simulator: type 'on' → Wallbox relay turns ON"
echo "2. In wallbox: type 'disable' → Relay turns OFF (safety override)"
echo "3. In wallbox: type 'enable' → Relay turns back ON"
echo "4. In simulator: type 'off' → Relay turns OFF"
echo ""

read -p "Press Enter to stop the background processes..."

kill $WALLBOX_PID $SIM_PID 2>/dev/null
echo ""
echo "Programs stopped."
echo "Logs saved in:"
echo "  - /tmp/wallbox.log"
echo "  - /tmp/simulator.log"
