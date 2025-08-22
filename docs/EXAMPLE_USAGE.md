# TPX3 Serval IOC Usage Examples

## Starting the IOC

1. **Build the IOC**:
   ```bash
   cd /epics/iocs/serval
   make clean
   make
   ```

2. **Start the IOC**:
   ```bash
   cd iocBoot/ioctpx3serval
   ../../bin/linux-x86_64/tpx3serval st.cmd
   ```

## Basic Control via EPICS PVs

### Start the Serval Application
```bash
# Using caput from command line
caput TPX3-TEST:Serval:START 1

# Or using caget to check status
caget TPX3-TEST:Serval:STATUS
```

### Stop the Serval Application
```bash
caput TPX3-TEST:Serval:START 0
```

### Check Application Status
```bash
# Check if running
caget TPX3-TEST:Serval:STATUS

# Get process ID
caget TPX3-TEST:Serval:PROCESS_ID

# Get command line
caget TPX3-TEST:Serval:COMMAND_LINE

# Get error messages
caget TPX3-TEST:Serval:ERROR_MSG
```

## Configuration Examples

### Enable/Disable Command Options

The IOC now supports enable/disable controls for each command option via `_ENABLE` PVs. This allows you to control which options are included in the generated command line.

#### Basic Enable/Disable Usage
```bash
# Enable HTTP logging option
caput TPX3-TEST:Serval:HTTP_LOG_ENABLE 1
caput -S TPX3-TEST:Serval:HTTP_LOG "/tmp/serval_http.log"

# Disable HTTP port option
caput TPX3-TEST:Serval:HTTP_PORT_ENABLE 0

# Enable pipeline options
caput TPX3-TEST:Serval:UDP_RECEIVERS_ENABLE 1
caput TPX3-TEST:Serval:UDP_RECEIVERS 4
caput TPX3-TEST:Serval:FRAME_ASSEMBLERS_ENABLE 1
caput TPX3-TEST:Serval:FRAME_ASSEMBLERS 2

# Check generated command line
caget TPX3-TEST:Serval:COMMAND_LINE
```

#### Default Enabled Options
The following options are enabled by default:
- `HTTP_PORT_ENABLE` = 1 (HTTP port will be included)
- `TCP_PORT_ENABLE` = 1 (TCP port will be included)
- `RESOURCE_POOL_SIZE_ENABLE` = 1 (Resource pool size will be included)
- `JAR_FILE_ENABLE` = 1 (JAR file path will be included)

All other options are disabled by default and must be explicitly enabled.

#### Advanced Enable/Disable Examples
```bash
# Enable SPIDR configuration
caput TPX3-TEST:Serval:SPIDR_NET_ENABLE 1
caput -S TPX3-TEST:Serval:SPIDR_NET "192.168.1.0"
caput TPX3-TEST:Serval:TCP_IP_ENABLE 1
caput -S TPX3-TEST:Serval:TCP_IP "192.168.1.100"

# Enable resource pool options
caput TPX3-TEST:Serval:IMAGE_POOL_SIZE_ENABLE 1
caput TPX3-TEST:Serval:IMAGE_POOL_SIZE 1048576
caput TPX3-TEST:Serval:INTEGRATION_POOL_SIZE_ENABLE 1
caput TPX3-TEST:Serval:INTEGRATION_POOL_SIZE 2097152

# Enable debug options
caput TPX3-TEST:Serval:TCP_DEBUG_ENABLE 1
caput -S TPX3-TEST:Serval:TCP_DEBUG "/tmp/serval_debug.log"

# Verify command line includes enabled options
caget TPX3-TEST:Serval:COMMAND_LINE
```

### Set HTTP Port to 8082
```bash
caput TPX3-TEST:Serval:HTTP_PORT 8082
```

### Enable HTTP Logging
```bash
caput -S TPX3-TEST:Serval:HTTP_LOG "/tmp/serval_http.log"
```

### Set SPIDR Network
```bash
caput -S TPX3-TEST:Serval:SPIDR_NET "192.168.1.0"
```

### Set TCP IP Address
```bash
caput -S TPX3-TEST:Serval:TCP_IP "192.168.1.100"
```

### Set Resource Pool Size
```bash
caput TPX3-TEST:Serval:RESOURCE_POOL_SIZE 1048576
```

### Enable Experimental Features
```bash
caput TPX3-TEST:Serval:EXPERIMENTAL 1
```

### Set Pipeline Parameters
```bash
# Set UDP receivers
caput TPX3-TEST:Serval:UDP_RECEIVERS 4

# Set frame assemblers
caput TPX3-TEST:Serval:FRAME_ASSEMBLERS 2

# Set ring buffer size
caput TPX3-TEST:Serval:RING_BUFFER_SIZE 1000
```

## Complete Configuration Example

Here's a complete example of configuring and starting the Serval application with enable/disable controls:

```bash
# 1. Enable and configure HTTP options
caput TPX3-TEST:Serval:HTTP_PORT_ENABLE 1
caput TPX3-TEST:Serval:HTTP_PORT 8082
caput TPX3-TEST:Serval:HTTP_LOG_ENABLE 1
caput -S TPX3-TEST:Serval:HTTP_LOG "/tmp/serval_http.log"

# 2. Enable and configure SPIDR options
caput TPX3-TEST:Serval:SPIDR_NET_ENABLE 1
caput -S TPX3-TEST:Serval:SPIDR_NET "192.168.1.0"
caput TPX3-TEST:Serval:TCP_IP_ENABLE 1
caput -S TPX3-TEST:Serval:TCP_IP "192.168.1.100"
caput TPX3-TEST:Serval:TCP_PORT_ENABLE 1
caput TPX3-TEST:Serval:TCP_PORT 50001

# 3. Enable and configure pipeline options
caput TPX3-TEST:Serval:UDP_RECEIVERS_ENABLE 1
caput TPX3-TEST:Serval:UDP_RECEIVERS 4
caput TPX3-TEST:Serval:FRAME_ASSEMBLERS_ENABLE 1
caput TPX3-TEST:Serval:FRAME_ASSEMBLERS 2
caput TPX3-TEST:Serval:RING_BUFFER_SIZE_ENABLE 1
caput TPX3-TEST:Serval:RING_BUFFER_SIZE 1000

# 4. Resource pool is enabled by default, but we can customize
caput TPX3-TEST:Serval:RESOURCE_POOL_SIZE 1048576

# 5. Start the application
caput TPX3-TEST:Serval:START 1

# 6. Verify it's running
caget TPX3-TEST:Serval:STATUS
caget TPX3-TEST:Serval:PROCESS_ID
caget TPX3-TEST:Serval:COMMAND_LINE
```

This will execute a command like:
```bash
java -jar ../../ASI/serval-4.1.1-rc1.jar --httpPort=8082 --httpLog=/tmp/serval_http.log --spidrNet=192.168.1.0 --tcpIp=192.168.1.100 --tcpPort=50001 --udpReceivers=4 --frameAssemblers=2 --ringBufferSize=1000 --resourcePoolSize=1048576
```

**Note**: Only enabled options appear in the command line. If you disable an option (set `_ENABLE` to 0), it won't be included even if you set a value for it.

## Monitoring and Debugging

### Check All Parameters
```bash
# List all PVs
caget TPX3-TEST:Serval:*

# Or check specific groups
caget TPX3-TEST:Serval:HTTP_*
caget TPX3-TEST:Serval:*POOL*
caget TPX3-TEST:Serval:*HANDLER*
```

### Monitor Status Changes
```bash
# Monitor status changes in real-time
camonitor TPX3-TEST:Serval:STATUS
camonitor TPX3-TEST:Serval:ERROR_MSG
```

### Check Process Information
```bash
# Get detailed process info
caget TPX3-TEST:Serval:PROCESS_ID
caget TPX3-TEST:Serval:COMMAND_LINE

# Check if process is actually running
ps aux | grep serval
```

## Troubleshooting

### Common Issues

1. **Process won't start**:
   - Check `ERROR_MSG` PV
   - Verify JAR file exists at specified path
   - Check Java runtime availability

2. **Process starts but immediately stops**:
   - Check Java application logs
   - Verify command line parameters
   - Check system resources

3. **IOC won't build**:
   - Verify EPICS_BASE is set
   - Check asyn support module availability
   - Ensure all source files are present

### Debug Commands

```bash
# Check IOC logs
tail -f /tmp/ioc.log

# Check Java process
ps aux | grep java

# Check network ports
netstat -tlnp | grep 8081

# Check file permissions
ls -la ../../ASI/serval-4.1.1-rc1.jar
```

## Integration with Control Systems

The IOC can be integrated with:
- **Control System Studio (CSS)**
- **Phoebus**
- **EPICS Channel Access clients**
- **Custom Python/Java applications**

Example Python client:
```python
from epics import caget, caput

# Start Serval
caput('TPX3-TEST:Serval:START', 1)

# Check status
status = caget('TPX3-TEST:Serval:STATUS')
print(f"Serval status: {status}")

# Get process info
pid = caget('TPX3-TEST:Serval:PROCESS_ID')
print(f"Process ID: {pid}")
```
