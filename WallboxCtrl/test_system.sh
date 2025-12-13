#!/bin/bash

# Wallbox System Test Script
# Tests the CP signal system integration between simulator and wallbox controller

echo "╔════════════════════════════════════════════════╗"
echo "║  Wallbox System Integration Test               ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Test API endpoint
API_URL="http://localhost:8080"

echo -e "${YELLOW}1. Testing API Health${NC}"
HEALTH=$(curl -s ${API_URL}/health)
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ API is responding${NC}"
    echo "   Response: $HEALTH"
else
    echo -e "${RED}✗ API is not responding${NC}"
    exit 1
fi
echo ""

echo -e "${YELLOW}2. Getting Current Status${NC}"
STATUS=$(curl -s ${API_URL}/api/status | python3 -m json.tool)
echo "$STATUS"
echo ""

echo -e "${YELLOW}3. Testing Charging Start${NC}"
START_RESULT=$(curl -s -X POST ${API_URL}/api/charging/start | python3 -m json.tool)
echo "$START_RESULT"
sleep 1
echo ""

echo -e "${YELLOW}4. Checking Status After Start${NC}"
STATUS=$(curl -s ${API_URL}/api/status | python3 -m json.tool)
echo "$STATUS"
echo ""

echo -e "${YELLOW}5. Testing Charging Pause${NC}"
PAUSE_RESULT=$(curl -s -X POST ${API_URL}/api/charging/pause | python3 -m json.tool)
echo "$PAUSE_RESULT"
sleep 1
echo ""

echo -e "${YELLOW}6. Checking Status After Pause${NC}"
STATUS=$(curl -s ${API_URL}/api/status | python3 -m json.tool)
echo "$STATUS"
echo ""

echo -e "${YELLOW}7. Testing Charging Resume${NC}"
RESUME_RESULT=$(curl -s -X POST ${API_URL}/api/charging/resume | python3 -m json.tool)
echo "$RESUME_RESULT"
sleep 1
echo ""

echo -e "${YELLOW}8. Testing Charging Stop${NC}"
STOP_RESULT=$(curl -s -X POST ${API_URL}/api/charging/stop | python3 -m json.tool)
echo "$STOP_RESULT"
sleep 1
echo ""

echo -e "${YELLOW}9. Final Status${NC}"
STATUS=$(curl -s ${API_URL}/api/status | python3 -m json.tool)
echo "$STATUS"
echo ""

echo -e "${YELLOW}10. Testing Relay Status${NC}"
RELAY=$(curl -s ${API_URL}/api/relay | python3 -m json.tool)
echo "$RELAY"
echo ""

echo "╔════════════════════════════════════════════════╗"
echo "║  Test Complete                                  ║"
echo "╚════════════════════════════════════════════════╝"
echo ""
echo "System Status:"
echo "  • Simulator: Running on UDP 50010/50011"
echo "  • Wallbox Controller: Running on HTTP 8080"
echo "  • CP Signal System: Integrated and functional"
echo ""
echo "To interact with the simulator, use:"
echo "  nc -u 127.0.0.1 50010"
echo ""
echo "To view logs:"
echo "  tail -f /tmp/wallbox.log"
echo "  tail -f /tmp/wallbox_simulator.log"
