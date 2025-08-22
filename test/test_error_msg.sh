#!/bin/bash

# Test script for ERROR_MSG PV functionality
# This script tests if the ERROR_MSG PV is properly updating

echo "=== ERROR_MSG PV Test Script ==="
echo ""

# Check if IOC is running
echo "1. Checking if IOC is running..."
if pgrep -f "tpx3serval" > /dev/null; then
    echo "   ✓ IOC is running"
else
    echo "   ✗ IOC is not running"
    echo "   Please start the IOC first using: ./runServal.sh"
    exit 1
fi

echo ""
echo "2. Testing ERROR_MSG PV access..."
echo "   Current ERROR_MSG value:"
caget TPX3-TEST:Serval:ERROR_MSG

echo ""
echo "3. Testing ERROR_MSG PV write (should fail - read-only)..."
caput TPX3-TEST:Serval:ERROR_MSG "Test message" 2>/dev/null || echo "   ✓ PV is read-only (correct)"

echo ""
echo "4. Testing other PVs to trigger ERROR_MSG updates..."
echo "   Setting HTTP_PORT to trigger status update..."
caput TPX3-TEST:Serval:HTTP_PORT 8082

echo ""
echo "5. Checking ERROR_MSG again..."
sleep 1
caget TPX3-TEST:Serval:ERROR_MSG

echo ""
echo "6. Testing enable/disable functionality..."
echo "   Disabling HTTP_PORT..."
caput TPX3-TEST:Serval:HTTP_PORT_ENABLE 0
sleep 1
caget TPX3-TEST:Serval:ERROR_MSG

echo ""
echo "7. Re-enabling HTTP_PORT..."
caput TPX3-TEST:Serval:HTTP_PORT_ENABLE 1
sleep 1
caget TPX3-TEST:Serval:ERROR_MSG

echo ""
echo "=== Test Complete ==="
echo ""
echo "If ERROR_MSG is updating with different messages, the PV is working correctly."
echo "If you see 'Channel connect timed out' errors, the IOC may not be running."
