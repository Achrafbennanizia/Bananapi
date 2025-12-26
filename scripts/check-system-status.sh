#!/bin/bash
# Check complete system status

echo "╔══════════════════════════════════════════════════════════╗"
echo "║  Wallbox System Status Check                            ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""

BANANA_PI="bananapi"
BANANA_PI_IP="192.168.178.34"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "1. Banana Pi Wallbox API ($BANANA_PI_IP)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Check if wallbox process is running
WALLBOX_PROCS=$(ssh $BANANA_PI 'pgrep -f wallbox_control_api | wc -l' 2>/dev/null)
if [ "$WALLBOX_PROCS" -gt 0 ]; then
    echo -e "   Process: ${GREEN}✅ Running ($WALLBOX_PROCS processes)${NC}"
else
    echo -e "   Process: ${RED}❌ Not running${NC}"
fi

# Check API endpoint
API_STATUS=$(curl -s -o /dev/null -w "%{http_code}" --max-time 2 http://$BANANA_PI_IP:8080/api/status 2>/dev/null)
if [ "$API_STATUS" = "200" ]; then
    echo -e "   API: ${GREEN}✅ Responding (HTTP $API_STATUS)${NC}"
    API_DATA=$(curl -s http://$BANANA_PI_IP:8080/api/status 2>/dev/null)
    echo "   State: $(echo $API_DATA | grep -o '"state":"[^"]*"' | cut -d'"' -f4)"
    echo "   Charging: $(echo $API_DATA | grep -o '"charging":[^,}]*' | cut -d':' -f2)"
else
    echo -e "   API: ${RED}❌ Not responding${NC}"
fi
echo ""

echo "2. Mac Simulator"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Check simulator process
SIMULATOR_PID=$(pgrep -f "simulator" | head -1)
if [ -n "$SIMULATOR_PID" ]; then
    echo -e "   Process: ${GREEN}✅ Running (PID: $SIMULATOR_PID)${NC}"
else
    echo -e "   Process: ${RED}❌ Not running${NC}"
fi

# Check UDP port
UDP_PORT=$(lsof -i:50011 -sTCP:LISTEN 2>/dev/null || lsof -i:50011 2>/dev/null)
if [ -n "$UDP_PORT" ]; then
    echo -e "   UDP Port 50011: ${GREEN}✅ Bound${NC}"
else
    echo -e "   UDP Port 50011: ${YELLOW}⚠️  Not bound${NC}"
fi
echo ""

echo "3. React Web App"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Check React process
REACT_PID=$(lsof -ti:3000 2>/dev/null)
if [ -n "$REACT_PID" ]; then
    echo -e "   Process: ${GREEN}✅ Running (PID: $REACT_PID)${NC}"
    echo "   URL: http://localhost:3000"
else
    echo -e "   Process: ${RED}❌ Not running${NC}"
fi
echo ""

echo "4. Network Configuration"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

MAC_IP=$(ipconfig getifaddr en0 2>/dev/null || ipconfig getifaddr en1 2>/dev/null)
echo "   Mac IP: $MAC_IP"
echo "   Banana Pi IP: $BANANA_PI_IP"

# Test connectivity
if ping -c 1 -W 1 $BANANA_PI_IP > /dev/null 2>&1; then
    echo -e "   Connectivity: ${GREEN}✅ Can reach Banana Pi${NC}"
else
    echo -e "   Connectivity: ${RED}❌ Cannot reach Banana Pi${NC}"
fi
echo ""

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Summary
WALLBOX_OK=$([[ "$WALLBOX_PROCS" -gt 0 ]] && [[ "$API_STATUS" = "200" ]] && echo "yes" || echo "no")
SIMULATOR_OK=$([[ -n "$SIMULATOR_PID" ]] && echo "yes" || echo "no")
REACT_OK=$([[ -n "$REACT_PID" ]] && echo "yes" || echo "no")

if [[ "$WALLBOX_OK" = "yes" ]] && [[ "$SIMULATOR_OK" = "yes" ]] && [[ "$REACT_OK" = "yes" ]]; then
    echo -e "${GREEN}✅ All systems operational!${NC}"
elif [[ "$WALLBOX_OK" = "yes" ]] && [[ "$REACT_OK" = "yes" ]]; then
    echo -e "${YELLOW}⚠️  Partial: Wallbox and Web OK, Simulator needs attention${NC}"
elif [[ "$WALLBOX_OK" = "yes" ]]; then
    echo -e "${YELLOW}⚠️  Partial: Only Wallbox OK${NC}"
else
    echo -e "${RED}❌ System not fully operational${NC}"
fi

echo ""
echo "To start the complete system: ./start-complete-system.sh"
