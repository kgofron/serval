#!/bin/bash

echo "🧪 Testing TPX3 Serval IOC Build..."

# Change to the project root directory (parent of test folder)
cd "$(dirname "$0")/.."

# Check if we're in the right directory
if [ ! -f "Makefile" ]; then
    echo "Error: Makefile not found. Please ensure this script is in the test/ folder of the serval IOC project."
    exit 1
fi

# Check EPICS environment
if [ -z "$EPICS_BASE" ]; then
    echo "Warning: EPICS_BASE environment variable not set"
    echo "This may cause build failures if EPICS is not in the default location"
fi

# Try to build
echo "🧹 Cleaning previous build..."
make clean

if [ $? -eq 0 ]; then
    echo "✅ Clean completed successfully"
else
    echo "❌ Clean failed"
    exit 1
fi

echo "🔨 Building IOC..."
make

if [ $? -eq 0 ]; then
    echo "✅ Build completed successfully"
    echo "🎉 Build test completed successfully!"
    echo "IOC binary available in bin/linux-x86_64/tpx3serval"
else
    echo "❌ Build failed. Please check the error messages above."
    exit 1
fi
