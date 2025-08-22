# TPX3 Serval Enable/Disable Functionality Implementation

## Overview

This document describes the implementation of enable/disable functionality for command options in the TPX3 Serval IOC driver. Each configuration option now has a corresponding `_ENABLE` PV that controls whether that option is included in the command line when starting the Serval process.

## Implementation Details

### 1. Database Changes

Added `_ENABLE` PVs for all command options in `tpx3servalApp/Db/tpx3serval.db`:

#### HTTP Configuration
- `HTTP_LOG_ENABLE` - Controls `--httpLog` option (default: Disabled)
- `HTTP_PORT_ENABLE` - Controls `--httpPort` option (default: Enabled)

#### SPIDR Configuration  
- `SPIDR_NET_ENABLE` - Controls `--spidrNet` option (default: Disabled)
- `TCP_IP_ENABLE` - Controls `--tcpIp` option (default: Disabled)
- `TCP_PORT_ENABLE` - Controls `--tcpPort` option (default: Enabled)

#### Device Configuration
- `DEVICE_MASK_ENABLE` - Controls `--deviceMask` option (default: Disabled)

#### Pipeline Configuration
- `UDP_RECEIVERS_ENABLE` - Controls `--udpReceivers` option (default: Disabled)
- `FRAME_ASSEMBLERS_ENABLE` - Controls `--frameAssemblers` option (default: Disabled)
- `RING_BUFFER_SIZE_ENABLE` - Controls `--ringBufferSize` option (default: Disabled)
- `NETWORK_BUFFER_SIZE_ENABLE` - Controls `--networkBufferSize` option (default: Disabled)
- `FILE_WRITERS_ENABLE` - Controls `--fileWriters` option (default: Disabled)
- `CORRECTION_HANDLERS_ENABLE` - Controls `--correctionHandlers` option (default: Disabled)
- `PROCESSING_HANDLERS_ENABLE` - Controls `--processingHandlers` option (default: Disabled)

#### Resource Pool Configuration
- `RESOURCE_POOL_SIZE_ENABLE` - Controls `--resourcePoolSize` option (default: Enabled)
- `IMAGE_POOL_SIZE_ENABLE` - Controls `--imagePoolSize` option (default: Disabled)
- `INTEGRATION_POOL_SIZE_ENABLE` - Controls `--integrationPoolSize` option (default: Disabled)

#### Debug and Experimental Configuration
- `TCP_DEBUG_ENABLE` - Controls `--tcpDebug` option (default: Disabled)

#### JAR File Configuration
- `JAR_FILE_ENABLE` - Controls `--jarFile` option (default: Enabled)

### 2. Driver Code Changes

#### Header File (`tpx3servalDriver.h`)
- Added parameter indices for all `_ENABLE` PVs
- Added boolean member variables for all enable flags
- Updated `NUM_PARAMS` from 50 to 75

#### Implementation File (`tpx3servalDriver.cpp`)
- Added parameter creation for all `_ENABLE` PVs in constructor
- Initialized enable flags with appropriate default values
- Set initial PV values for all enable parameters
- Updated `writeInt32` method to handle enable parameter changes
- Completely rewrote `buildCommandString` method to check enable flags

### 3. Default Values

The following options are enabled by default (as they are commonly used):
- `HTTP_PORT_ENABLE` = 1 (Enabled)
- `TCP_PORT_ENABLE` = 1 (Enabled)  
- `RESOURCE_POOL_SIZE_ENABLE` = 1 (Enabled)
- `JAR_FILE_ENABLE` = 1 (Enabled)

All other options are disabled by default (set to 0).

### 4. Command Line Generation Logic

The `buildCommandString` method now:
1. Always starts with "java -jar"
2. Only includes the jar file path if `JAR_FILE_ENABLE` is true
3. Only includes `--httpPort` if `HTTP_PORT_ENABLE` is true
4. Only includes `--resourcePoolSize` if `RESOURCE_POOL_SIZE_ENABLE` is true
5. For all other options, checks both the enable flag AND the value:
   - String options: enabled AND not empty/default
   - Numeric options: enabled AND greater than 0 (or not equal to default)
   - Boolean options: always included if true (no enable flag needed)

### 5. Usage Examples

#### Enable HTTP logging:
```bash
caput TPX3:HTTP_LOG_ENABLE 1
caput TPX3:HTTP_LOG "/path/to/log"
```

#### Disable HTTP port:
```bash
caput TPX3:HTTP_PORT_ENABLE 0
```

#### Enable pipeline options:
```bash
caput TPX3:UDP_RECEIVERS_ENABLE 1
caput TPX3:UDP_RECEIVERS 4
caput TPX3:FRAME_ASSEMBLERS_ENABLE 1
caput TPX3:FRAME_ASSEMBLERS 2
```

#### Check generated command:
```bash
caget TPX3:COMMAND_LINE
```

## Benefits

1. **Flexibility**: Users can now control exactly which command options are included
2. **Cleaner Commands**: Only enabled options appear in the command line
3. **Consistent Interface**: All options follow the same enable/disable pattern
4. **Default Behavior**: Sensible defaults for commonly used options
5. **Runtime Control**: Options can be enabled/disabled without restarting the IOC

## Testing

Use the provided test script `test_enable_functionality.sh` to verify the functionality:

```bash
./test_enable_functionality.sh
```

## Notes

- The enable flags are stored as boolean values in the driver but exposed as integer PVs (0/1)
- Changes to enable flags take effect immediately but don't affect running processes
- The command line is regenerated each time `startProcess()` is called
- All enable flags are preserved across IOC restarts
