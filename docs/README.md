# TPX3 Serval IOC Documentation

Welcome to the TPX3 Serval IOC documentation. This folder contains comprehensive documentation for the EPICS IOC that controls the TPX3 Serval data acquisition system.

## 📚 **Documentation Index**

### **Core Documentation**
- **[README.md](../README.md)** - Main project overview and quick start guide
- **[Phoebus Screens](../tpx3servalApp/op/bob/README.md)** - Complete guide to the Phoebus graphical interface

### **Configuration & Usage**
- **[CONFIGURATION.md](CONFIGURATION.md)** - Detailed configuration options and parameters
- **[EXAMPLE_USAGE.md](EXAMPLE_USAGE.md)** - Practical examples and usage scenarios
- **[BUILD_SUCCESS.md](BUILD_SUCCESS.md)** - Build verification and testing results

### **Testing & Validation**
- **[test/README.md](../test/README.md)** - Testing documentation and scripts

### **Project Structure**
```
serval/
├── README.md                    # Main project overview
├── docs/                        # This documentation folder
│   ├── README.md               # Documentation index (this file)
│   ├── CONFIGURATION.md        # Configuration guide
│   ├── EXAMPLE_USAGE.md        # Usage examples
│   └── BUILD_SUCCESS.md        # Build verification
├── test/                        # Testing and validation scripts
│   ├── README.md               # Testing documentation
│   └── build_test.sh           # Automated build testing
├── tpx3servalApp/              # Main IOC application
│   ├── op/bob/                 # Phoebus screens
│   │   ├── README.md           # Phoebus screen documentation
│   │   ├── tpx3serval.bob      # Main Phoebus display file
│   │   └── test_screens.sh     # Screen validation script
│   ├── src/                    # C++ source code
│   └── Db/                     # EPICS database files
├── configure/                   # Build configuration
├── iocBoot/                    # IOC startup scripts
└── ASI/                        # Application files
```

## 🚀 **Quick Start**

### **1. Build the IOC**
```bash
make clean
make
```

### **2. Start the IOC**
```bash
cd iocBoot/ioctpx3serval
./st.cmd
```

### **3. Launch Phoebus Interface**
```bash
/epics/GUI/SNS/phoebus-4.7.4-SNAPSHOT/phoebus.sh
```
Then open: `tpx3servalApp/op/bob/tpx3serval.bob`

### **4. Control via EPICS**
```bash
caput TPX3-TEST:Serval:START 1    # Start serval
caput TPX3-TEST:Serval:START 0    # Stop serval
caget TPX3-TEST:Serval:STATUS     # Check status
```

## 📖 **Documentation Guide**

### **For New Users**
1. Start with **[README.md](../README.md)** for project overview
2. Read **[CONFIGURATION.md](CONFIGURATION.md)** to understand options
3. Follow **[EXAMPLE_USAGE.md](EXAMPLE_USAGE.md)** for practical examples
4. Use **[Phoebus Screens](../tpx3servalApp/op/bob/README.md)** for GUI interface

### **For Developers**
1. Review **[BUILD_SUCCESS.md](BUILD_SUCCESS.md)** for build verification
2. Check **[CONFIGURATION.md](CONFIGURATION.md)** for parameter details
3. Examine source code in `tpx3servalApp/src/`
4. Review database files in `tpx3servalApp/Db/`

### **For Operators**
1. Use **[Phoebus Screens](../tpx3servalApp/op/bob/README.md)** for daily operation
2. Reference **[EXAMPLE_USAGE.md](EXAMPLE_USAGE.md)** for common tasks
3. Check **[CONFIGURATION.md](CONFIGURATION.md)** for parameter explanations

## 🔧 **Key Features**

### **Complete Serval Support**
- **25+ command-line options** available as EPICS PVs
- **Real-time monitoring** of process status and command generation
- **Autotuning support** with proper handling of 0 values
- **Professional Phoebus interface** with logical organization

### **EPICS Integration**
- **Full EPICS 7.x compatibility**
- **Channel Access support** for remote monitoring
- **Database-driven configuration** with substitution support
- **Process management** with graceful startup/shutdown

### **Phoebus Interface**
- **1400x1200 screen** accommodating all parameters
- **Logical grouping** of related parameters
- **Real-time updates** of all PVs and command line
- **Professional appearance** with color-coded sections

## 📋 **Parameter Categories**

### **HTTP Configuration**
- HTTP port, HTTP log file path

### **SPIDR Configuration**
- SPIDR subnet, TCP IP, TCP port

### **Pipeline Configuration**
- UDP receivers, frame assemblers, ring buffer size
- Network buffer size, file writers, correction handlers
- Processing handlers, device mask

### **Resource Pools**
- Resource pool size, image pool size, integration pool size

### **Advanced Options**
- TCP debug log, release resources, experimental features

### **JAR File Configuration**
- Filename, path, and full path readback

## 🆘 **Getting Help**

### **Documentation Issues**
- Check this index for the right document
- Verify file paths and links
- Ensure you're reading the latest version

### **Technical Issues**
- Review **[BUILD_SUCCESS.md](BUILD_SUCCESS.md)** for build verification
- Check **[CONFIGURATION.md](CONFIGURATION.md)** for parameter details
- Follow **[EXAMPLE_USAGE.md](EXAMPLE_USAGE.md)** for troubleshooting

### **Phoebus Issues**
- See **[Phoebus Screens](../tpx3servalApp/op/bob/README.md)** for interface help
- Verify Phoebus path: `/epics/GUI/SNS/phoebus-4.7.4-SNAPSHOT/phoebus.sh`
- Check widget compatibility and screen validation

## 📊 **Version Information**

- **IOC Version**: 1.0
- **EPICS Version**: 7.x
- **Phoebus Compatibility**: 4.7.4+
- **Serval Version**: 4.1.1-rc1
- **Documentation Version**: 2.0
- **Last Updated**: August 2024

## ✅ **Documentation Status**

- ✅ **Complete Coverage** - All aspects of the IOC documented
- ✅ **User-Friendly** - Clear organization and navigation
- ✅ **Practical Examples** - Real-world usage scenarios
- ✅ **Technical Details** - Comprehensive configuration information
- ✅ **Build Verification** - Confirmed working implementation
- ✅ **Phoebus Integration** - Full screen documentation

---

**Note**: This documentation is maintained with the TPX3 Serval IOC project. For the most up-to-date information, always refer to the files in this repository.
