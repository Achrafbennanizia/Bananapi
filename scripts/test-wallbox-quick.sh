#!/bin/bash
# Quick Wallbox API Tests

API_BASE="http://192.168.178.34:8080"

echo "╔══════════════════════════════════════════════════════════╗"
echo "║         Wallbox API Quick Test Suite                    ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}1. Health Check${NC}"
curl -s $API_BASE/health && echo "" || echo "Failed"
echo ""

echo -e "${BLUE}2. Initial Status${NC}"
curl -s $API_BASE/api/status | python3 -m json.tool
echo ""

echo -e "${BLUE}3. Start Charging${NC}"
curl -s -X POST $API_BASE/api/charging/start | python3 -m json.tool
echo ""
sleep 2

echo -e "${BLUE}4. Check Status (should be CHARGING)${NC}"
curl -s $API_BASE/api/status | python3 -m json.tool
echo ""
sleep 2

echo -e "${BLUE}5. Pause Charging${NC}"
curl -s -X POST $API_BASE/api/charging/pause | python3 -m json.tool
echo ""
sleep 2

echo -e "${BLUE}6. Check Status (should be PAUSED)${NC}"
curl -s $API_BASE/api/status | python3 -m json.tool
echo ""
sleep 2

echo -e "${BLUE}7. Resume Charging${NC}"
curl -s -X POST $API_BASE/api/charging/resume | python3 -m json.tool
echo ""
sleep 2

echo -e "${BLUE}8. Check Status (should be CHARGING)${NC}"
curl -s $API_BASE/api/status | python3 -m json.tool
echo ""
sleep 2

echo -e "${BLUE}9. Stop Charging${NC}"
curl -s -X POST $API_BASE/api/charging/stop | python3 -m json.tool
echo ""
sleep 2

echo -e "${BLUE}10. Final Status (should be IDLE)${NC}"
curl -s $API_BASE/api/status | python3 -m json.tool
echo ""

echo -e "${GREEN}╔══════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║              ✅ All Tests Completed                      ║${NC}"
echo -e "${GREEN}╚══════════════════════════════════════════════════════════╝${NC}"
