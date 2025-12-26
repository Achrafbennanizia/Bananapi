#!/bin/bash
# Direct terminal interface to Banana Pi Wallbox Controller v4

echo "╔══════════════════════════════════════════════════════════╗"
echo "║    Wallbox v4 Interactive Terminal - Full Features      ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""
echo "Available commands:"
echo "  enable          - Enable wallbox"
echo "  disable         - Disable wallbox"
echo "  start           - Start charging session"
echo "  stop            - Stop charging"
echo "  pause           - Pause charging"
echo "  resume          - Resume charging"
echo "  status          - Show current status"
echo "  setrelay <pin>  - Change relay pin"
echo "  getpins         - Show pin configuration"
echo "  getudp          - Show UDP configuration"
echo "  help            - Show all commands"
echo "  quit/exit       - Exit terminal"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "Connecting to bananapi..."
echo ""

# SSH to Banana Pi and run v4 in interactive mode
ssh -t bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v4 --interactive'
