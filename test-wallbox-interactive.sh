#!/bin/bash
# Wallbox Testing Guide - Interactive Test Script

clear
echo "╔══════════════════════════════════════════════════════════╗"
echo "║         Wallbox Controller Testing Interface             ║"
echo "║              Banana Pi @ 192.168.178.34                  ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""

BANANA_PI_IP="192.168.178.34"
API_BASE="http://$BANANA_PI_IP:8080"

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

# Function to display status
show_status() {
    echo ""
    echo -e "${BLUE}━━━ Current Status ━━━${NC}"
    RESPONSE=$(curl -s $API_BASE/api/status)
    echo $RESPONSE | python3 -m json.tool 2>/dev/null || echo $RESPONSE
    echo ""
}

# Function to test endpoint
test_endpoint() {
    local method=$1
    local endpoint=$2
    local description=$3
    
    echo -e "${YELLOW}Testing: $description${NC}"
    echo "Command: curl -X $method $API_BASE$endpoint"
    
    if [ "$method" = "GET" ]; then
        RESPONSE=$(curl -s $API_BASE$endpoint)
    else
        RESPONSE=$(curl -s -X $method $API_BASE$endpoint)
    fi
    
    echo "Response:"
    echo $RESPONSE | python3 -m json.tool 2>/dev/null || echo $RESPONSE
    echo ""
    sleep 1
}

# Check if API is running
echo "1️⃣  Checking if Wallbox API is running..."
if curl -s --max-time 2 $API_BASE/health > /dev/null 2>&1; then
    echo -e "${GREEN}✅ Wallbox API is running${NC}"
else
    echo -e "${RED}❌ Wallbox API is not responding${NC}"
    echo ""
    echo "Start it with:"
    echo "  ssh bananapi 'cd ~/wallbox-src/build && ./wallbox_control_api > /tmp/wallbox_api.out 2>&1 &'"
    exit 1
fi

show_status

echo "╔══════════════════════════════════════════════════════════╗"
echo "║                    Test Menu                             ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""
echo "Choose a test:"
echo "  1) Get Status"
echo "  2) Start Charging"
echo "  3) Stop Charging"
echo "  4) Pause Charging"
echo "  5) Resume Charging"
echo "  6) Enable Wallbox"
echo "  7) Disable Wallbox"
echo "  8) Run All Tests (Full Sequence)"
echo "  9) Health Check"
echo "  0) Exit"
echo ""

while true; do
    read -p "Enter choice [0-9]: " choice
    
    case $choice in
        1)
            test_endpoint "GET" "/api/status" "Get Current Status"
            ;;
        2)
            test_endpoint "POST" "/api/charging/start" "Start Charging"
            show_status
            ;;
        3)
            test_endpoint "POST" "/api/charging/stop" "Stop Charging"
            show_status
            ;;
        4)
            test_endpoint "POST" "/api/charging/pause" "Pause Charging"
            show_status
            ;;
        5)
            test_endpoint "POST" "/api/charging/resume" "Resume Charging"
            show_status
            ;;
        6)
            test_endpoint "POST" "/api/wallbox/enable" "Enable Wallbox"
            show_status
            ;;
        7)
            test_endpoint "POST" "/api/wallbox/disable" "Disable Wallbox"
            show_status
            ;;
        8)
            echo -e "${BLUE}╔══════════════════════════════════════════════════════════╗${NC}"
            echo -e "${BLUE}║           Running Full Test Sequence                     ║${NC}"
            echo -e "${BLUE}╚══════════════════════════════════════════════════════════╝${NC}"
            echo ""
            
            test_endpoint "GET" "/api/status" "1. Initial Status"
            sleep 2
            
            test_endpoint "POST" "/api/charging/start" "2. Start Charging"
            sleep 2
            show_status
            
            test_endpoint "POST" "/api/charging/pause" "3. Pause Charging"
            sleep 2
            show_status
            
            test_endpoint "POST" "/api/charging/resume" "4. Resume Charging"
            sleep 2
            show_status
            
            test_endpoint "POST" "/api/charging/stop" "5. Stop Charging"
            sleep 2
            show_status
            
            echo -e "${GREEN}✅ Full test sequence completed!${NC}"
            echo ""
            ;;
        9)
            test_endpoint "GET" "/health" "Health Check"
            ;;
        0)
            echo ""
            echo "Exiting..."
            exit 0
            ;;
        *)
            echo -e "${RED}Invalid choice. Please enter 0-9.${NC}"
            ;;
    esac
    
    echo ""
    read -p "Press Enter to continue..."
    echo ""
done
