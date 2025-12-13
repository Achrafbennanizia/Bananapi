#!/bin/bash
# Stop Wallbox Control System

echo "🛑 Stopping Wallbox Control System"
echo "==================================="

# Find and kill wallbox process
PIDS=$(pgrep -f wallbox_control_v3)

if [ -z "$PIDS" ]; then
    echo "✅ No wallbox process running"
    exit 0
fi

echo "Found wallbox process(es): $PIDS"
echo "Sending SIGTERM..."

for PID in $PIDS; do
    kill -TERM $PID 2>/dev/null
    echo "  Stopped process $PID"
done

# Wait for graceful shutdown
sleep 2

# Force kill if still running
PIDS=$(pgrep -f wallbox_control_v3)
if [ -n "$PIDS" ]; then
    echo "Force killing remaining processes..."
    for PID in $PIDS; do
        kill -9 $PID 2>/dev/null
    done
fi

echo "✅ Wallbox stopped"
