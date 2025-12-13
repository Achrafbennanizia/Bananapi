#!/bin/bash
# Install Wallbox as systemd service
# Run with: sudo ./install-service.sh

if [ "$EUID" -ne 0 ]; then
    echo "❌ Please run as root: sudo ./install-service.sh"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASE_DIR="$(dirname "$SCRIPT_DIR")"
BIN_DIR="$BASE_DIR/bin"

echo "📦 Installing Wallbox System as systemd service"
echo "================================================"

# Create systemd service file
cat > /etc/systemd/system/wallbox.service << SERVICEEOF
[Unit]
Description=Wallbox Control System v3.0
After=network.target

[Service]
Type=simple
User=$SUDO_USER
WorkingDirectory=$BIN_DIR
ExecStart=$BIN_DIR/wallbox_control_v3
Restart=on-failure
RestartSec=5
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
SERVICEEOF

echo "✅ Created /etc/systemd/system/wallbox.service"

# Reload systemd
systemctl daemon-reload
echo "✅ Reloaded systemd"

# Enable service
systemctl enable wallbox.service
echo "✅ Enabled wallbox service (auto-start on boot)"

echo ""
echo "Installation complete! 🎉"
echo ""
echo "Usage:"
echo "  sudo systemctl start wallbox    # Start service"
echo "  sudo systemctl stop wallbox     # Stop service"
echo "  sudo systemctl status wallbox   # Check status"
echo "  sudo systemctl restart wallbox  # Restart service"
echo "  journalctl -u wallbox -f        # View logs"
