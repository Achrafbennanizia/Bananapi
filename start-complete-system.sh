#!/bin/bash
# Complete System Startup Script
# Starts: Wallbox on Banana Pi + Simulator on Mac + React Website

echo "╔══════════════════════════════════════════════════════════╗"
echo "║  Wallbox Complete System Startup                        ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

BANANA_PI="bananapi"
BANANA_PI_IP="192.168.178.34"

echo "1️⃣  Starting Wallbox API on Banana Pi ($BANANA_PI_IP)..."
ssh $BANANA_PI 'killall wallbox_control_api 2>/dev/null; cd ~/wallbox-src/build && nohup ./wallbox_control_api > /tmp/wallbox_api.out 2>&1 &'
sleep 2

# Check if wallbox started
WALLBOX_PID=$(ssh $BANANA_PI 'pgrep -f wallbox_control_api')
if [ -n "$WALLBOX_PID" ]; then
    echo -e "   ${GREEN}✅ Wallbox API running (PID: $WALLBOX_PID)${NC}"
else
    echo -e "   ${RED}❌ Failed to start Wallbox API${NC}"
    exit 1
fi
echo ""

echo "2️⃣  Testing Wallbox API endpoint..."
API_RESPONSE=$(curl -s --max-time 3 http://$BANANA_PI_IP:8080/api/status)
if [ $? -eq 0 ] && [ -n "$API_RESPONSE" ]; then
    echo -e "   ${GREEN}✅ API responding: http://$BANANA_PI_IP:8080${NC}"
    echo "   Response: $API_RESPONSE"
else
    echo -e "   ${RED}❌ API not responding${NC}"
    exit 1
fi
echo ""

echo "3️⃣  Starting Simulator on Mac..."
# Kill existing simulator
pkill -f "wallbox.*simulator" 2>/dev/null
lsof -ti:50011 | xargs kill -9 2>/dev/null

# Start simulator in background
cd /Users/achraf/pro/PJMT/WallboxCtrl/build
nohup ./simulator > /tmp/wallbox_simulator.log 2>&1 &
SIMULATOR_PID=$!
sleep 2

if ps -p $SIMULATOR_PID > /dev/null; then
    echo -e "   ${GREEN}✅ Simulator started (PID: $SIMULATOR_PID)${NC}"
    echo -e "   ${YELLOW}⚠️  Configure simulator manually:${NC}"
    echo "      > setudp $BANANA_PI_IP 50011 50010"
    echo "      > status"
else
    echo -e "   ${RED}❌ Failed to start simulator${NC}"
fi
echo ""

echo "4️⃣  Starting React Web App..."
# Check if React is already running
if lsof -ti:3000 > /dev/null 2>&1; then
    echo -e "   ${YELLOW}⚠️  React already running on port 3000${NC}"
else
    cd /Users/achraf/pro/PJMT/wallbox-react-app
    BROWSER=none npm start > /tmp/react_app.log 2>&1 &
    REACT_PID=$!
    
    # Wait for React to start
    echo "   Waiting for React to start..."
    for i in {1..15}; do
        if lsof -ti:3000 > /dev/null 2>&1; then
            echo -e "   ${GREEN}✅ React app started${NC}"
            break
        fi
        sleep 1
    done
fi
echo ""

echo "╔══════════════════════════════════════════════════════════╗"
echo "║  System Status                                           ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""
echo "🖥️  Banana Pi Wallbox:"
echo "   API: http://$BANANA_PI_IP:8080/api/status"
echo "   SSH: ssh $BANANA_PI"
echo "   Logs: ssh $BANANA_PI 'tail -f /tmp/wallbox_api.out'"
echo ""
echo "🔧 Mac Simulator:"
echo "   UDP: Listening on port 50011, sending to $BANANA_PI_IP:50010"
echo "   Logs: tail -f /tmp/wallbox_simulator.log"
echo "   To configure: Send commands to the simulator terminal"
echo ""
echo "🌐 React Web App:"
echo "   URL: http://localhost:3000"
echo "   API Target: $BANANA_PI_IP:8080"
echo "   Logs: tail -f /tmp/react_app.log"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "📋 Quick Commands:"
echo "   Check all: ./check-system-status.sh"
echo "   Stop all: ./stop-all.sh"
echo "   API test: curl http://$BANANA_PI_IP:8080/api/status"
echo ""
echo -e "${GREEN}✅ System startup complete!${NC}"
echo ""
echo "Open http://localhost:3000 in your browser to access the wallbox control panel."
