#!/bin/bash

echo "Testing TPX3 Serval IOC Phoebus Screens..."

# Check if we're in the right directory
if [ ! -f "tpx3serval.bob" ]; then
    echo "Error: tpx3serval.bob not found. Please run this script from the bob directory."
    exit 1
fi

# Check if Phoebus is available at the specified location
PHOEBUS_PATH="/epics/GUI/SNS/phoebus-4.7.4-SNAPSHOT/phoebus.sh"
if [ -f "$PHOEBUS_PATH" ]; then
    echo "✅ Phoebus found at: $PHOEBUS_PATH"
    echo "   To launch Phoebus Display Builder, run: $PHOEBUS_PATH"
else
    echo "❌ Phoebus not found at: $PHOEBUS_PATH"
    echo "   Please verify the path or update the script with the correct location."
fi

# Also check if phoebus is in PATH as a fallback
if command -v phoebus &> /dev/null; then
    echo "✅ Phoebus is also available in PATH"
else
    echo "⚠️  Phoebus not found in PATH"
fi

# Validate BOB file structure
echo "🔍 Validating BOB file structure..."

# Check for required XML elements
if grep -q "<?xml version=" tpx3serval.bob; then
    echo "✅ XML declaration found"
else
    echo "❌ XML declaration missing"
fi

if grep -q "<display version=" tpx3serval.bob; then
    echo "✅ Display element found"
else
    echo "❌ Display element missing"
fi

if grep -q "<macros>" tpx3serval.bob; then
    echo "✅ Macros section found"
else
    echo "❌ Macros section missing"
fi

# Check for required PVs
echo "🔍 Checking for required PVs..."

required_pvs=(
    "START"
    "STATUS"
    "PROCESS_ID"
    "ERROR_MSG"
    "HTTP_PORT"
    "HTTP_LOG"
    "SPIDR_NET"
    "TCP_IP"
    "TCP_PORT"
    "UDP_RECEIVERS"
    "FRAME_ASSEMBLERS"
    "RING_BUFFER_SIZE"
    "NETWORK_BUFFER_SIZE"
    "FILE_WRITERS"
    "CORRECTION_HANDLERS"
    "PROCESSING_HANDLERS"
    "DEVICE_MASK"
    "RESOURCE_POOL_SIZE"
    "IMAGE_POOL_SIZE"
    "INTEGRATION_POOL_SIZE"
    "TCP_DEBUG"
    "RELEASE_RESOURCES"
    "EXPERIMENTAL"
    "JarFileName"
    "JarFilePath"
    "JarFile_RBV"
    "COMMAND_LINE"
)

missing_pvs=()
for pv in "${required_pvs[@]}"; do
    if grep -q "$pv" tpx3serval.bob; then
        echo "✅ PV $pv found"
    else
        echo "❌ PV $pv missing"
        missing_pvs+=("$pv")
    fi
done

# Check for widget types
echo "🔍 Checking widget types..."

widget_types=(
    "slide_button"
    "led"
    "spinner"
    "textupdate"
    "label"
    "group"
)

for widget in "${widget_types[@]}"; do
    if grep -q "type=\"$widget\"" tpx3serval.bob; then
        echo "✅ Widget type $widget found"
    else
        echo "❌ Widget type $widget missing"
    fi
done

# Summary
echo ""
echo "📊 Summary:"
echo "============"

if [ ${#missing_pvs[@]} -eq 0 ]; then
    echo "✅ All required PVs are present"
else
    echo "❌ Missing PVs: ${missing_pvs[*]}"
fi

echo ""
echo "🎯 To use these screens:"
echo "1. Launch Phoebus Display Builder: $PHOEBUS_PATH"
echo "2. Open tpx3serval.bob"
echo "3. Ensure the IOC is running"
echo "4. Configure parameters as needed"
echo "5. Use the Start/Stop button to control Serval"

echo ""
echo "📁 Screen files created:"
ls -la *.bob *.md

echo ""
echo "✨ Phoebus screen testing complete!"
