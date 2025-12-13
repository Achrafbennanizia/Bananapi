#!/bin/bash
# Start simulator configured for Banana Pi wallbox

echo "Starting simulator for remote wallbox at 192.168.178.34..."
echo ""
echo "Simulator will listen on port 50011 and send to Banana Pi:50010"
echo ""
echo "Interactive commands available:"
echo "  setudp 192.168.178.34 50011 50010  - Configure for Banana Pi"
echo "  on      - Turn main contactor ON"
echo "  off     - Turn main contactor OFF"
echo "  charge  - Start charging"
echo "  stop    - Stop charging"
echo "  status  - Show current status"
echo ""
echo "Run this after simulator starts:"
echo "  > setudp 192.168.178.34 50011 50010"
echo ""

./simulator
