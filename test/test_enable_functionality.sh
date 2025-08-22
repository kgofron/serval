#!/bin/bash

echo "Testing TPX3 Serval Enable/Disable Functionality"
echo "================================================"
echo ""

# Test 1: Default configuration (some options enabled, some disabled)
echo "Test 1: Default Configuration"
echo "Expected: HTTP_PORT and RESOURCE_POOL_SIZE should be enabled by default"
echo "Command should include: --httpPort=8081 --resourcePoolSize=524288"
echo ""

# Test 2: Enable HTTP_LOG option
echo "Test 2: Enable HTTP_LOG option"
echo "Set HTTP_LOG_ENABLE=1 and HTTP_LOG=/path/to/log"
echo "Command should include: --httpLog=/path/to/log"
echo ""

# Test 3: Disable HTTP_PORT option
echo "Test 3: Disable HTTP_PORT option"
echo "Set HTTP_PORT_ENABLE=0"
echo "Command should NOT include: --httpPort=8081"
echo ""

# Test 4: Enable multiple pipeline options
echo "Test 4: Enable multiple pipeline options"
echo "Set UDP_RECEIVERS_ENABLE=1, UDP_RECEIVERS=4"
echo "Set FRAME_ASSEMBLERS_ENABLE=1, FRAME_ASSEMBLERS=2"
echo "Command should include: --udpReceivers=4 --frameAssemblers=2"
echo ""

# Test 5: Enable TCP_DEBUG option
echo "Test 5: Enable TCP_DEBUG option"
echo "Set TCP_DEBUG_ENABLE=1 and TCP_DEBUG=/path/to/debug"
echo "Command should include: --tcpDebug=/path/to/debug"
echo ""

# Test 6: Disable JAR_FILE option
echo "Test 6: Disable JAR_FILE option"
echo "Set JAR_FILE_ENABLE=0"
echo "Command should NOT include the jar file path"
echo ""

# Test 7: Test SPIDR configuration options
echo "Test 7: Test SPIDR configuration options"
echo "Set SPIDR_NET_ENABLE=1, SPIDR_NET=192.168.1.0"
echo "Set TCP_IP_ENABLE=1, TCP_IP=192.168.1.100"
echo "Command should include: --spidrNet=192.168.1.0 --tcpIp=192.168.1.100"
echo ""

# Test 8: Test resource pool options
echo "Test 8: Test resource pool options"
echo "Set IMAGE_POOL_SIZE_ENABLE=1, IMAGE_POOL_SIZE=1048576"
echo "Set INTEGRATION_POOL_SIZE_ENABLE=1, INTEGRATION_POOL_SIZE=2097152"
echo "Command should include: --imagePoolSize=1048576 --integrationPoolSize=2097152"
echo ""

# Test 9: Test device and processing options
echo "Test 9: Test device and processing options"
echo "Set DEVICE_MASK_ENABLE=1, DEVICE_MASK=1"
echo "Set PROCESSING_HANDLERS_ENABLE=1, PROCESSING_HANDLERS=8"
echo "Command should include: --deviceMask=1 --processingHandlers=8"
echo ""

# Test 10: Test buffer and handler options
echo "Test 10: Test buffer and handler options"
echo "Set RING_BUFFER_SIZE_ENABLE=1, RING_BUFFER_SIZE=65536"
echo "Set CORRECTION_HANDLERS_ENABLE=1, CORRECTION_HANDLERS=4"
echo "Command should include: --ringBufferSize=65536 --correctionHandlers=4"
echo ""

echo "To test these scenarios:"
echo "1. Start the IOC"
echo "2. Use caput to set the _ENABLE PVs to 1 (enabled) or 0 (disabled)"
echo "3. Use caput to set the configuration values"
echo "4. Check the COMMAND_LINE PV to see the generated command"
echo "5. Use the START PV to execute the command"
echo ""

echo "Example commands:"
echo "caput TPX3:HTTP_LOG_ENABLE 1"
echo "caput TPX3:HTTP_LOG \"/path/to/log\""
echo "caput TPX3:HTTP_PORT_ENABLE 0"
echo "caput TPX3:UDP_RECEIVERS_ENABLE 1"
echo "caput TPX3:UDP_RECEIVERS 4"
echo "caget TPX3:COMMAND_LINE"
echo ""

echo "Quick Test Commands (run these in sequence):"
echo "---------------------------------------------"
echo "# Test 1: Check default command"
echo "caget TPX3:COMMAND_LINE"
echo ""
echo "# Test 2: Enable HTTP logging"
echo "caput TPX3:HTTP_LOG_ENABLE 1"
echo "caput TPX3:HTTP_LOG \"/tmp/serval.log\""
echo "caget TPX3:COMMAND_LINE"
echo ""
echo "# Test 3: Disable HTTP port"
echo "caput TPX3:HTTP_PORT_ENABLE 0"
echo "caget TPX3:COMMAND_LINE"
echo ""
echo "# Test 4: Enable pipeline options"
echo "caput TPX3:UDP_RECEIVERS_ENABLE 1"
echo "caput TPX3:UDP_RECEIVERS 4"
echo "caput TPX3:FRAME_ASSEMBLERS_ENABLE 1"
echo "caput TPX3:FRAME_ASSEMBLERS 2"
echo "caget TPX3:COMMAND_LINE"
echo ""
echo "# Test 5: Reset to defaults"
echo "caput TPX3:HTTP_PORT_ENABLE 1"
echo "caput TPX3:HTTP_LOG_ENABLE 0"
echo "caput TPX3:UDP_RECEIVERS_ENABLE 0"
echo "caput TPX3:FRAME_ASSEMBLERS_ENABLE 0"
echo "caget TPX3:COMMAND_LINE"
echo ""

echo "Notes:"
echo "- All _ENABLE PVs are binary (0=disabled, 1=enabled)"
echo "- Changes take effect immediately"
echo "- The COMMAND_LINE PV updates in real-time"
echo "- Only enabled options appear in the generated command"
echo "- Default enabled options: HTTP_PORT, TCP_PORT, RESOURCE_POOL_SIZE, JAR_FILE"
echo ""
