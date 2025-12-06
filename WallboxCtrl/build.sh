#!/bin/bash

echo "=== Building Wallbox Control System ==="
echo ""

cd "$(dirname "$0")"

# Create build directory if it doesn't exist
mkdir -p build

# Build with CMake
echo "Building wallbox_control..."
cd build
cmake .. && make

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Build successful!"
    echo "Executable: $(pwd)/wallbox_control"
    echo ""
    echo "To run: ./build/wallbox_control"
else
    echo ""
    echo "✗ Build failed!"
    exit 1
fi
