#!/bin/bash

# Test script for Wallbox Control System
# This script demonstrates the communication between simulator and wallbox_ctrl

echo "=== Wallbox Control System Test ==="
echo ""
echo "Starting wallbox_ctrl in background..."
cd /Users/achraf/pro/PJMT/WallboxCtrl/src
./wallbox_ctrl > /tmp/wallbox_ctrl.log 2>&1 &
WALLBOX_PID=$!
echo "wallbox_ctrl started with PID: $WALLBOX_PID"

sleep 1

echo "Starting simulator in background..."
./simulator > /tmp/simulator.log 2>&1 &
SIM_PID=$!
echo "Simulator started with PID: $SIM_PID"

sleep 2

echo ""
echo "=== Testing ON command ==="
echo "on" | timeout 2 nc -u 127.0.0.1 50010 || true
sleep 1

echo "Checking wallbox_ctrl log for relay ON..."
tail -5 /tmp/wallbox_ctrl.log | grep -E "Relay|mainContactor" || echo "No relay change yet"

sleep 2

echo ""
echo "=== Testing OFF command ==="  
echo "Sending OFF via direct UDP packet..."
# Send a state packet with mainContactor=0
printf '\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' | nc -u 127.0.0.1 50010
sleep 1

echo ""
echo "=== Logs ==="
echo "--- Last 10 lines from wallbox_ctrl ---"
tail -10 /tmp/wallbox_ctrl.log

echo ""
echo "--- Last 10 lines from simulator ---"
tail -10 /tmp/simulator.log

echo ""
echo "=== Cleaning up ==="
kill $WALLBOX_PID $SIM_PID 2>/dev/null
echo "Processes terminated"

echo ""
echo "Test complete!"
echo "Log files:"
echo "  - /tmp/wallbox_ctrl.log"
echo "  - /tmp/simulator.log"
