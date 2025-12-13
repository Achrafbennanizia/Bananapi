#!/bin/bash
# Stop all system components

echo "🛑 Stopping Wallbox Complete System..."
echo ""

# Stop React
echo "Stopping React app..."
lsof -ti:3000 | xargs kill -9 2>/dev/null
echo "✓ React stopped"

# Stop Simulator
echo "Stopping Simulator..."
pkill -f "simulator" 2>/dev/null
lsof -ti:50011 | xargs kill -9 2>/dev/null
echo "✓ Simulator stopped"

# Stop Wallbox on Banana Pi
echo "Stopping Wallbox API on Banana Pi..."
ssh bananapi 'killall wallbox_control_api 2>/dev/null'
echo "✓ Wallbox stopped"

echo ""
echo "✅ All systems stopped"
