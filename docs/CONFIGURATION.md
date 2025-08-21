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

### Required Parameters (Always Included)
- `--httpPort=8081` - HTTP port for client connections
- `--resourcePoolSize=524288` - Resource pool size for packet handling

### Optional Parameters (Added When Set)
- `--httpLog=<path>` - HTTP access log file path
- `--spidrNet=<subnet>` - SPIDR subnet (default: autodiscover)
- `--tcpIp=<ip>` - TCP IP address (default: autodiscover)
- `--tcpPort=<port>` - TCP port (default: 50000)
- `--deviceMask=<mask>` - Device disable mask (default: unset)
- `--udpReceivers=<count>` - UDP receiver count (default: autotuning)
- `--frameAssemblers=<count>` - Frame assembler count (default: autotuning)
- `--ringBufferSize=<size>` - Ring buffer size (default: autotuning)
- `--networkBufferSize=<size>` - Network buffer size (default: autotuning)
- `--fileWriters=<count>` - File writer thread count (default: autotuning)
- `--correctionHandlers=<count>` - Correction handler count (default: autotuning)
- `--processingHandlers=<count>` - Processing handler count (default: autotuning)
- `--imagePoolSize=<size>` - Image pool size (default: autotuning)
- `--integrationPoolSize=<size>` - Integration pool size (default: autotuning)
- `--tcpDebug=<path>` - TCP debug log path
- `--releaseResources` - Release resources after measurement
- `--experimental` - Enable experimental features

## EPICS PV Structure

All PVs follow the pattern: `$(P)$(R)<PARAMETER_NAME>`

Where:
- `P` = `TPX3-TEST:` (configurable in st.cmd)
- `R` = `Serval:` (configurable in st.cmd)

Example PVs:
- `TPX3-TEST:Serval:START` - Start/Stop control
- `TPX3-TEST:Serval:HTTP_PORT` - HTTP port configuration
- `TPX3-TEST:Serval:RESOURCE_POOL_SIZE` - Resource pool size
- `TPX3-TEST:Serval:STATUS` - Application status

## Autotuning Behavior

Parameters set to 0 or empty will use Serval's autotuning defaults:
- `UDP_RECEIVERS=0` → `--udpReceivers` not added (autotuning)
- `FRAME_ASSEMBLERS=0` → `--frameAssemblers` not added (autotuning)
- `RING_BUFFER_SIZE=0` → `--ringBufferSize` not added (autotuning)

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
