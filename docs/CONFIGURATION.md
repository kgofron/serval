# TPX3 Serval IOC Configuration Guide

## Quick Start

1. **Build the IOC**:
   ```bash
   make clean
   make
   ```

2. **Run the IOC**:
   ```bash
   cd iocBoot/ioctpx3serval
   ../../bin/linux-x86_64/tpx3serval st.cmd
   ```

## Default Command

The IOC will execute the following command by default:
```bash
java -jar ../../ASI/serval-4.1.1-rc1.jar --httpPort=8081 --resourcePoolSize=524288
```

## Configuration Options

### Enable/Disable Controls

Each command option now has a corresponding `_ENABLE` PV that controls whether that option is included in the command line:

- `HTTP_PORT_ENABLE` - Controls `--httpPort` option
- `HTTP_LOG_ENABLE` - Controls `--httpLog` option
- `SPIDR_NET_ENABLE` - Controls `--spidrNet` option
- `TCP_IP_ENABLE` - Controls `--tcpIp` option
- `TCP_PORT_ENABLE` - Controls `--tcpPort` option
- `DEVICE_MASK_ENABLE` - Controls `--deviceMask` option
- `UDP_RECEIVERS_ENABLE` - Controls `--udpReceivers` option
- `FRAME_ASSEMBLERS_ENABLE` - Controls `--frameAssemblers` option
- `RING_BUFFER_SIZE_ENABLE` - Controls `--ringBufferSize` option
- `NETWORK_BUFFER_SIZE_ENABLE` - Controls `--networkBufferSize` option
- `FILE_WRITERS_ENABLE` - Controls `--fileWriters` option
- `CORRECTION_HANDLERS_ENABLE` - Controls `--correctionHandlers` option
- `PROCESSING_HANDLERS_ENABLE` - Controls `--processingHandlers` option
- `RESOURCE_POOL_SIZE_ENABLE` - Controls `--resourcePoolSize` option
- `IMAGE_POOL_SIZE_ENABLE` - Controls `--imagePoolSize` option
- `INTEGRATION_POOL_SIZE_ENABLE` - Controls `--integrationPoolSize` option
- `TCP_DEBUG_ENABLE` - Controls `--tcpDebug` option
- `JAR_FILE_ENABLE` - Controls JAR file path inclusion

### Default Enabled Options
The following options are enabled by default (set to 1):
- `HTTP_PORT_ENABLE` = 1 - HTTP port will be included
- `TCP_PORT_ENABLE` = 1 - TCP port will be included
- `RESOURCE_POOL_SIZE_ENABLE` = 1 - Resource pool size will be included
- `JAR_FILE_ENABLE` = 1 - JAR file path will be included

All other options are disabled by default (set to 0) and must be explicitly enabled.

### Required Parameters (Always Included When Enabled)
- `--httpPort=8081` - HTTP port for client connections (if HTTP_PORT_ENABLE=1)
- `--resourcePoolSize=524288` - Resource pool size for packet handling (if RESOURCE_POOL_SIZE_ENABLE=1)

### Optional Parameters (Added When Enabled AND Set)
- `--httpLog=<path>` - HTTP access log file path (if HTTP_LOG_ENABLE=1 AND path not empty)
- `--spidrNet=<subnet>` - SPIDR subnet (if SPIDR_NET_ENABLE=1 AND not autodiscover)
- `--tcpIp=<ip>` - TCP IP address (if TCP_IP_ENABLE=1 AND not autodiscover)
- `--tcpPort=<port>` - TCP port (if TCP_PORT_ENABLE=1 AND not default 50000)
- `--deviceMask=<mask>` - Device disable mask (if DEVICE_MASK_ENABLE=1 AND not 0)
- `--udpReceivers=<count>` - UDP receiver count (if UDP_RECEIVERS_ENABLE=1 AND > 0)
- `--frameAssemblers=<count>` - Frame assembler count (if FRAME_ASSEMBLERS_ENABLE=1 AND > 0)
- `--ringBufferSize=<size>` - Ring buffer size (if RING_BUFFER_SIZE_ENABLE=1 AND > 0)
- `--networkBufferSize=<size>` - Network buffer size (if NETWORK_BUFFER_SIZE_ENABLE=1 AND > 0)
- `--fileWriters=<count>` - File writer thread count (if FILE_WRITERS_ENABLE=1 AND > 0)
- `--correctionHandlers=<count>` - Correction handler count (if CORRECTION_HANDLERS_ENABLE=1 AND > 0)
- `--processingHandlers=<count>` - Processing handler count (if PROCESSING_HANDLERS_ENABLE=1 AND > 0)
- `--imagePoolSize=<size>` - Image pool size (if IMAGE_POOL_SIZE_ENABLE=1 AND > 0)
- `--integrationPoolSize=<size>` - Integration pool size (if INTEGRATION_POOL_SIZE_ENABLE=1 AND > 0)
- `--tcpDebug=<path>` - TCP debug log path (if TCP_DEBUG_ENABLE=1 AND path not empty)
- `--releaseResources` - Release resources after measurement (always included if true)
- `--experimental` - Enable experimental features (always included if true)

## EPICS PV Structure

All PVs follow the pattern: `$(P)$(R)<PARAMETER_NAME>`

Where:
- `P` = `TPX3-TEST:` (configurable in st.cmd)
- `R` = `Serval:` (configurable in st.cmd)

### Standard Parameter PVs
Example PVs:
- `TPX3-TEST:Serval:START` - Start/Stop control
- `TPX3-TEST:Serval:HTTP_PORT` - HTTP port configuration
- `TPX3-TEST:Serval:RESOURCE_POOL_SIZE` - Resource pool size
- `TPX3-TEST:Serval:STATUS` - Application status

### Enable/Disable PVs
Each parameter has a corresponding `_ENABLE` PV:
- `TPX3-TEST:Serval:HTTP_PORT_ENABLE` - Enable/disable HTTP port option
- `TPX3-TEST:Serval:UDP_RECEIVERS_ENABLE` - Enable/disable UDP receivers option
- `TPX3-TEST:Serval:FRAME_ASSEMBLERS_ENABLE` - Enable/disable frame assemblers option
- And so on for all 15 command options...

**Note**: Enable PVs are binary (0=disabled, 1=enabled). Only enabled options appear in the generated command line.

## Enable/Disable Functionality Usage

### Basic Usage
To enable a command option:
```bash
# Enable HTTP logging
caput TPX3-TEST:Serval:HTTP_LOG_ENABLE 1
caput -S TPX3-TEST:Serval:HTTP_LOG "/tmp/serval.log"

# Enable pipeline options
caput TPX3-TEST:Serval:UDP_RECEIVERS_ENABLE 1
caput TPX3-TEST:Serval:UDP_RECEIVERS 4
```

To disable a command option:
```bash
# Disable HTTP port (will not appear in command line)
caput TPX3-TEST:Serval:HTTP_PORT_ENABLE 0
```

### Checking Generated Command
```bash
# View the current command line
caget TPX3-TEST:Serval:COMMAND_LINE

# Monitor command line changes in real-time
camonitor TPX3-TEST:Serval:COMMAND_LINE
```

### Reset to Defaults
```bash
# Reset all enable flags to defaults
caput TPX3-TEST:Serval:HTTP_PORT_ENABLE 1
caput TPX3-TEST:Serval:TCP_PORT_ENABLE 1
caput TPX3-TEST:Serval:RESOURCE_POOL_SIZE_ENABLE 1
caput TPX3-TEST:Serval:JAR_FILE_ENABLE 1
# All other _ENABLE PVs default to 0
```

## Autotuning Behavior

Parameters set to 0 or empty will use Serval's autotuning defaults:
- `UDP_RECEIVERS=0` → `--udpReceivers` not added (autotuning)
- `FRAME_ASSEMBLERS=0` → `--frameAssemblers` not added (autotuning)
- `RING_BUFFER_SIZE=0` → `--ringBufferSize` not added (autotuning)

**Note**: Even if autotuning values are set, the option will only appear in the command line if its corresponding `_ENABLE` PV is set to 1.

## Process Management

The IOC automatically:
- Monitors the Serval process status
- Handles graceful shutdown (SIGTERM then SIGKILL)
- Cleans up processes on IOC shutdown
- Provides process ID and command line readback

## Error Handling

- Process start/stop failures are reported in `ERROR_MSG`
- Invalid parameter values are validated and reported
- Process termination is detected and status updated
- Cleanup is performed on IOC shutdown

## Customization

To modify default values:
1. Edit `tpx3servalApp/src/tpx3servalDriver.cpp` constructor
2. Rebuild the IOC
3. Or modify the database file `tpx3servalApp/Db/tpx3serval.db`

## Monitoring

Use these PVs to monitor the system:
- `STATUS` - Running/Stopped status
- `PROCESS_ID` - Current Java process ID
- `COMMAND_LINE` - Full command being executed
- `ERROR_MSG` - Status and error messages
- `JarFile_RBV` - Full path to JAR file
