#!/bin/bash
# Quick test to verify status exchange is working

echo "=========================================="
echo "Testing Status Exchange"
echo "=========================================="
echo ""

cd /Users/achraf/pro/PJMT/WallboxCtrl/build

echo "Starting simulator in background..."
./simulator > /tmp/simulator_test.log 2>&1 &
SIM_PID=$!
sleep 1

echo "Starting wallbox in background..."
./wallbox_control_v4 > /tmp/wallbox_test.log 2>&1 &
WB_PID=$!
sleep 2

echo ""
echo "Checking communication..."
echo "=========================================="

# Check simulator log
echo ""
echo "Simulator log (first 20 lines):"
head -20 /tmp/simulator_test.log

echo ""
echo "Wallbox log (first 20 lines):"
head -20 /tmp/wallbox_test.log

echo ""
echo "=========================================="
echo "Looking for status exchange messages..."
echo "=========================================="

# Check for key messages
if grep -q "First message received" /tmp/simulator_test.log; then
    echo "✓ Simulator is RECEIVING messages from wallbox"
else
    echo "✗ Simulator NOT receiving messages from wallbox"
fi

if grep -q "Starting to send status" /tmp/wallbox_test.log; then
    echo "✓ Wallbox is SENDING messages to simulator"
else
    echo "✗ Wallbox NOT sending messages"
fi

if grep -q "Relay state changed" /tmp/simulator_test.log; then
    echo "✓ Simulator detected relay state change"
else
    echo "⚠  No relay state changes detected (normal if wallbox state unchanged)"
fi

echo ""
echo "=========================================="
echo "Full logs available at:"
echo "  Simulator: /tmp/simulator_test.log"
echo "  Wallbox:   /tmp/wallbox_test.log"
echo "=========================================="

# Cleanup
echo ""
echo "Cleaning up test processes..."
kill $SIM_PID $WB_PID 2>/dev/null
sleep 1

echo "Test complete!"
