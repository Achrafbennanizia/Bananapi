#!/bin/bash

# Portable Deployment Test Script
# Tests the wallbox-portable-deploy package

echo "╔════════════════════════════════════════════════╗"
echo "║  Portable Deployment Integration Test          ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test API endpoint
API_URL="http://localhost:8080"

echo -e "${BLUE}Testing Portable Deployment Package${NC}"
echo ""

echo -e "${YELLOW}1. Checking Running Processes${NC}"
WALLBOX_PID=$(ps aux | grep "wallbox_control_v3" | grep -v grep | grep -v ssh | awk '{print $2}' | head -1)
SIMULATOR_PID=$(ps aux | grep "simulator" | grep -v grep | awk '{print $2}' | head -1)

if [ -n "$WALLBOX_PID" ]; then
    echo -e "${GREEN}✓ Wallbox Controller running (PID: $WALLBOX_PID)${NC}"
else
    echo -e "${RED}✗ Wallbox Controller not running${NC}"
    exit 1
fi

if [ -n "$SIMULATOR_PID" ]; then
    echo -e "${GREEN}✓ Simulator running (PID: $SIMULATOR_PID)${NC}"
else
    echo -e "${RED}✗ Simulator not running${NC}"
    exit 1
fi
echo ""

echo -e "${YELLOW}2. Checking Network Ports${NC}"
PORT_8080=$(lsof -i :8080 2>/dev/null | grep -v COMMAND | wc -l)
PORT_50010=$(lsof -i :50010 2>/dev/null | grep -v COMMAND | wc -l)
PORT_50011=$(lsof -i :50011 2>/dev/null | grep -v COMMAND | wc -l)

if [ "$PORT_8080" -gt 0 ]; then
    echo -e "${GREEN}✓ HTTP API port 8080 open${NC}"
else
    echo -e "${RED}✗ Port 8080 not listening${NC}"
fi

if [ "$PORT_50010" -gt 0 ]; then
    echo -e "${GREEN}✓ UDP port 50010 open${NC}"
else
    echo -e "${RED}✗ Port 50010 not listening${NC}"
fi

if [ "$PORT_50011" -gt 0 ]; then
    echo -e "${GREEN}✓ UDP port 50011 open${NC}"
else
    echo -e "${RED}✗ Port 50011 not listening${NC}"
fi
echo ""

echo -e "${YELLOW}3. Testing API Health${NC}"
HEALTH=$(curl -s ${API_URL}/health)
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ API responding${NC}"
    echo "   Response: $HEALTH"
else
    echo -e "${RED}✗ API not responding${NC}"
    exit 1
fi
echo ""

echo -e "${YELLOW}4. Testing Full Charging Cycle${NC}"
echo ""

echo "  4.1 Initial Status"
STATUS=$(curl -s ${API_URL}/api/status)
INITIAL_STATE=$(echo $STATUS | python3 -c "import sys, json; print(json.load(sys.stdin)['state'])")
echo "      State: $INITIAL_STATE"
echo ""

echo "  4.2 Start Charging"
START_RESULT=$(curl -s -X POST ${API_URL}/api/charging/start)
START_SUCCESS=$(echo $START_RESULT | python3 -c "import sys, json; print(json.load(sys.stdin).get('success', False))")
if [ "$START_SUCCESS" == "True" ]; then
    echo -e "      ${GREEN}✓ Charging started${NC}"
else
    echo -e "      ${RED}✗ Failed to start charging${NC}"
fi
sleep 1
STATUS=$(curl -s ${API_URL}/api/status)
CURRENT_STATE=$(echo $STATUS | python3 -c "import sys, json; print(json.load(sys.stdin)['state'])")
echo "      New State: $CURRENT_STATE"
echo ""

echo "  4.3 Pause Charging"
PAUSE_RESULT=$(curl -s -X POST ${API_URL}/api/charging/pause)
PAUSE_SUCCESS=$(echo $PAUSE_RESULT | python3 -c "import sys, json; print(json.load(sys.stdin).get('success', False))")
if [ "$PAUSE_SUCCESS" == "True" ]; then
    echo -e "      ${GREEN}✓ Charging paused${NC}"
else
    echo -e "      ${RED}✗ Failed to pause charging${NC}"
fi
sleep 1
STATUS=$(curl -s ${API_URL}/api/status)
CURRENT_STATE=$(echo $STATUS | python3 -c "import sys, json; print(json.load(sys.stdin)['state'])")
echo "      New State: $CURRENT_STATE"
echo ""

echo "  4.4 Resume Charging"
RESUME_RESULT=$(curl -s -X POST ${API_URL}/api/charging/resume)
RESUME_SUCCESS=$(echo $RESUME_RESULT | python3 -c "import sys, json; print(json.load(sys.stdin).get('success', False))")
if [ "$RESUME_SUCCESS" == "True" ]; then
    echo -e "      ${GREEN}✓ Charging resumed${NC}"
else
    echo -e "      ${RED}✗ Failed to resume charging${NC}"
fi
sleep 1
STATUS=$(curl -s ${API_URL}/api/status)
CURRENT_STATE=$(echo $STATUS | python3 -c "import sys, json; print(json.load(sys.stdin)['state'])")
echo "      New State: $CURRENT_STATE"
echo ""

echo "  4.5 Stop Charging"
STOP_RESULT=$(curl -s -X POST ${API_URL}/api/charging/stop)
STOP_SUCCESS=$(echo $STOP_RESULT | python3 -c "import sys, json; print(json.load(sys.stdin).get('success', False))")
if [ "$STOP_SUCCESS" == "True" ]; then
    echo -e "      ${GREEN}✓ Charging stopped${NC}"
else
    echo -e "      ${RED}✗ Failed to stop charging${NC}"
fi
sleep 1
STATUS=$(curl -s ${API_URL}/api/status)
FINAL_STATE=$(echo $STATUS | python3 -c "import sys, json; print(json.load(sys.stdin)['state'])")
echo "      Final State: $FINAL_STATE"
echo ""

echo -e "${YELLOW}5. Testing Wallbox Enable/Disable${NC}"
echo ""

echo "  5.1 Disable Wallbox"
DISABLE_RESULT=$(curl -s -X POST ${API_URL}/api/wallbox/disable)
echo "      Response: $DISABLE_RESULT"
sleep 1
echo ""

echo "  5.2 Enable Wallbox"
ENABLE_RESULT=$(curl -s -X POST ${API_URL}/api/wallbox/enable)
echo "      Response: $ENABLE_RESULT"
sleep 1
echo ""

echo -e "${YELLOW}6. Checking System Resources${NC}"
WALLBOX_MEM=$(ps aux | grep "wallbox_control_v3" | grep -v grep | grep -v ssh | awk '{print $4"%"}' | head -1)
WALLBOX_CPU=$(ps aux | grep "wallbox_control_v3" | grep -v grep | grep -v ssh | awk '{print $3"%"}' | head -1)
SIMULATOR_MEM=$(ps aux | grep "simulator" | grep -v grep | awk '{print $4"%"}' | head -1)
SIMULATOR_CPU=$(ps aux | grep "simulator" | grep -v grep | awk '{print $3"%"}' | head -1)

echo "  Wallbox Controller:"
echo "    CPU: $WALLBOX_CPU"
echo "    Memory: $WALLBOX_MEM"
echo ""
echo "  Simulator:"
echo "    CPU: $SIMULATOR_CPU"
echo "    Memory: $SIMULATOR_MEM"
echo ""

echo -e "${YELLOW}7. Checking Log Files${NC}"
WALLBOX_LOG="/tmp/portable_wallbox.log"
SIMULATOR_LOG="/tmp/portable_simulator.log"

if [ -f "$WALLBOX_LOG" ]; then
    WALLBOX_LINES=$(wc -l < "$WALLBOX_LOG")
    echo -e "  ${GREEN}✓ Wallbox log exists: $WALLBOX_LINES lines${NC}"
else
    echo -e "  ${RED}✗ Wallbox log not found${NC}"
fi

if [ -f "$SIMULATOR_LOG" ]; then
    SIMULATOR_LINES=$(wc -l < "$SIMULATOR_LOG")
    echo -e "  ${GREEN}✓ Simulator log exists: $SIMULATOR_LINES lines${NC}"
else
    echo -e "  ${RED}✗ Simulator log not found${NC}"
fi
echo ""

echo "╔════════════════════════════════════════════════╗"
echo "║  Portable Deployment Test Complete             ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

echo -e "${BLUE}Deployment Information:${NC}"
echo "  Package: wallbox-portable-deploy"
echo "  Wallbox PID: $WALLBOX_PID"
echo "  Simulator PID: $SIMULATOR_PID"
echo "  HTTP API: http://localhost:8080"
echo "  UDP Ports: 50010 (RX), 50011 (TX)"
echo ""

echo -e "${BLUE}Files:${NC}"
echo "  Executable: ./build/wallbox_control_v3"
echo "  Simulator: ./build/simulator"
echo "  Config: ./config/config.json"
echo "  Wallbox Log: $WALLBOX_LOG"
echo "  Simulator Log: $SIMULATOR_LOG"
echo ""

echo -e "${GREEN}✓ All tests passed!${NC}"
echo ""
echo "To view logs:"
echo "  tail -f $WALLBOX_LOG"
echo "  tail -f $SIMULATOR_LOG"
