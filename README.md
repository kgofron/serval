# EPICS TPX3 Serval IOC

This is an EPICS IOC for controlling the TPX3 Serval data acquisition system. It provides a complete interface to manage the Serval Java application with all available configuration options.

## Overview

The TPX3 Serval IOC is designed to control the `serval-4.1.1-rc1.jar` application through EPICS Process Variables (PVs). It allows users to:

- Start/stop the Serval application
- Configure all Serval command-line parameters
- Monitor the application status and process information
- Manage the application lifecycle through EPICS

## Default Configuration

The IOC is configured with the following default values as specified in the requirements:

- **HTTP Port**: 8081
- **Resource Pool Size**: 524288
- **JAR File**: `serval-4.1.1-rc1.jar`
- **JAR Path**: `../../ASI`

## Available Process Variables (PVs)

### Control PVs
- `START`: Start/Stop the Serval application (0=Stop, 1=Start)

### HTTP Configuration
- `HTTP_LOG`: File path to store HTTP access log
- `HTTP_PORT`: HTTP port number (default: 8081)

### SPIDR Configuration
- `SPIDR_NET`: Subnet for SPIDR discovery (default: autodiscover)
- `TCP_IP`: IP address for SPIDR (default: autodiscover)
- `TCP_PORT`: Port for SPIDR (default: 50000)

### Device Configuration
- `DEVICE_MASK`: Binary mask to disable chips (default: unset)

### Pipeline Configuration
- `UDP_RECEIVERS`: Number of UDP receivers (default: autotuning)
- `FRAME_ASSEMBLERS`: Number of frame assemblers (default: autotuning)
- `RING_BUFFER_SIZE`: Ring buffer size in frames (default: autotuning)
- `NETWORK_BUFFER_SIZE`: Network buffer size in bytes (default: autotuning)
- `FILE_WRITERS`: Number of file writer threads (default: autotuning)
- `CORRECTION_HANDLERS`: Number of correction handler threads (default: autotuning)
- `PROCESSING_HANDLERS`: Number of processing handler threads (default: autotuning)

### Resource Pool Configuration
- `RESOURCE_POOL_SIZE`: Resource pool size (default: 524288)
- `IMAGE_POOL_SIZE`: Image resource pool size (default: autotuning)
- `INTEGRATION_POOL_SIZE`: Integration image resource pool size (default: autotuning)

### Debug and Experimental
- `TCP_DEBUG`: File path for TCP debug log
- `RELEASE_RESOURCES`: Release resources after measurement (default: disabled)
- `EXPERIMENTAL`: Enable experimental options (default: disabled)

### JAR File Configuration
- `JarFileName`: JAR filename
- `JarFilePath`: JAR file path
- `JarFile_RBV`: Readback of full JAR file path

### Status and Monitoring
- `STATUS`: Application status (0=Stopped, 1=Running)
- `PROCESS_ID`: Current process ID
- `COMMAND_LINE`: Full command line being executed
- `ERROR_MSG`: Error messages and status updates

## Building the IOC

1. Ensure EPICS base and asyn support are properly configured
2. Run `make` in the top-level directory
3. The IOC binary will be built in `bin/linux-x86_64/`

### **Automated Build Testing**
For automated build testing, use the test script:
```bash
cd test
./build_test.sh
```

This script will clean, build, and verify the IOC build process.

## Running the IOC

1. Navigate to `iocBoot/ioctpx3serval/`
2. Run `../../bin/linux-x86_64/tpx3serval st.cmd`
3. The IOC will start and load the database

## Configuration

The IOC can be configured through the database file `tpx3serval.db` or by modifying the default values in the driver source code. All parameters support runtime modification through EPICS PVs.

## Architecture

The IOC consists of:

- **Main IOC Application**: `tpx3servalMain.cpp` - Entry point and signal handling
- **Driver**: `tpx3servalDriver.cpp/h` - Core driver implementation with asynPortDriver
- **Database**: `tpx3serval.db` - EPICS database definition
- **Support Library**: `tpx3servalSupport.dbd` - Support module registration

## Dependencies

- EPICS Base
- asyn support module
- Linux system with Java runtime

## Notes

- The IOC automatically manages the Serval Java process lifecycle
- Process monitoring and cleanup are handled automatically
- All Serval command-line options are exposed as EPICS PVs
- The default command executed is: `java -jar serval-4.1.1-rc1.jar --httpPort=8081 --resourcePoolSize=524288`
- Additional parameters are added based on PV values (0 or empty values use autotuning defaults)

## Documentation

This project includes comprehensive documentation organized in the `docs/` folder:

- **[docs/README.md](docs/README.md)** - Complete documentation index and navigation
- **[docs/CONFIGURATION.md](docs/CONFIGURATION.md)** - Detailed configuration options and parameters
- **[docs/EXAMPLE_USAGE.md](docs/EXAMPLE_USAGE.md)** - Practical examples and usage scenarios  
- **[docs/BUILD_SUCCESS.md](docs/BUILD_SUCCESS.md)** - Build verification and testing results
- **[tpx3servalApp/op/bob/README.md](tpx3servalApp/op/bob/README.md)** - Phoebus screen documentation

For detailed information, start with the [documentation index](docs/README.md) which provides navigation to all available documentation.