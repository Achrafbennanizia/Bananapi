#!/bin/bash
# Test passwordless SSH connection to Banana Pi

echo "╔════════════════════════════════════════════════════════╗"
echo "║  Testing Passwordless SSH Access to Banana Pi         ║"
echo "╚════════════════════════════════════════════════════════╝"
echo ""

echo "1. Hostname check..."
HOSTNAME=$(ssh bananapi hostname 2>&1)
if [ $? -eq 0 ]; then
    echo "   ✅ Connected to: $HOSTNAME"
else
    echo "   ❌ Connection failed"
    exit 1
fi
echo ""

echo "2. Wallbox status..."
PROCESSES=$(ssh bananapi 'pgrep -f wallbox_control_v4 | wc -l' 2>&1)
echo "   Wallbox processes running: $PROCESSES"
if [ "$PROCESSES" -gt 0 ]; then
    echo "   ✅ Wallbox is running"
else
    echo "   ⚠️  Wallbox not running"
fi
echo ""

echo "3. System uptime..."
ssh bananapi 'uptime' | sed 's/^/   /'
echo ""

echo "4. Network configuration..."
ssh bananapi "grep 'udp_send_address' ~/wallbox-src/build/config.json" | sed 's/^/   /'
echo ""

echo "5. API availability..."
API_STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://192.168.178.34:8080/api/status 2>&1)
if [ "$API_STATUS" = "200" ]; then
    echo "   ✅ API responding (HTTP $API_STATUS)"
else
    echo "   ⚠️  API status: $API_STATUS"
fi
echo ""

echo "╔════════════════════════════════════════════════════════╗"
echo "║  ✅ Passwordless SSH authentication working!          ║"
echo "╚════════════════════════════════════════════════════════╝"
echo ""
echo "SSH Key: ~/.ssh/bananapi_key"
echo "Config: ~/.ssh/config (aliases: bananapi, bananapim5)"
echo "Usage: ssh bananapi"
