#!/bin/bash

# Start only the API server in background
# Run the simulator interactively in a separate terminal

export WALLBOX_MODE=dev

echo "🚀 Starting API server in background..."

cd WallboxCtrl/build
./wallbox_control_v4 > /tmp/wallbox_api.log 2>&1 &
PID_API=$!

sleep 2

if ! ps -p $PID_API > /dev/null; then
    echo "❌ Failed to start API server"
    cat /tmp/wallbox_api.log
    exit 1
fi

echo "✅ API server running (PID: $PID_API) on port 8080"
echo "📝 Log: /tmp/wallbox_api.log"
echo ""
echo "Now run the simulator interactively in another terminal:"
echo "  cd WallboxCtrl/build && ./simulator"
echo ""
echo "Press Ctrl+C to stop the API server"

# Wait for Ctrl+C
trap "echo ''; echo 'Stopping API server...'; kill $PID_API 2>/dev/null; exit 0" INT TERM
wait $PID_API
