# TPX3 Serval IOC Testing

This folder contains testing and validation scripts for the TPX3 Serval IOC.

## 📁 **Contents**

### **Build Testing**
- **`build_test.sh`** - Automated build testing script for the IOC

### **Functionality Testing**
- **`test_enable_functionality.sh`** - Test script for enable/disable functionality

## 🧪 **Build Testing**

### **build_test.sh**
A simple shell script that automates the build process and verifies the build was successful.

## 🧪 **Functionality Testing**

### **test_enable_functionality.sh**
A comprehensive test script that demonstrates and validates the enable/disable functionality for command options.

#### **What It Tests**
1. **Default Configuration** - Verifies which options are enabled by default
2. **Enable/Disable Controls** - Tests all 15 _ENABLE PVs
3. **Command Line Generation** - Shows how options appear/disappear from command
4. **Real-time Updates** - Demonstrates immediate effect of changes
5. **Reset Functionality** - Shows how to return to default settings

#### **Usage**
```bash
cd test
./test_enable_functionality.sh
```

#### **Test Scenarios**
- **Test 1**: Default configuration (HTTP_PORT and RESOURCE_POOL_SIZE enabled)
- **Test 2**: Enable HTTP_LOG option
- **Test 3**: Disable HTTP_PORT option
- **Test 4**: Enable multiple pipeline options
- **Test 5**: Enable TCP_DEBUG option
- **Test 6**: Disable JAR_FILE option
- **Test 7**: Test SPIDR configuration options
- **Test 8**: Test resource pool options
- **Test 9**: Test device and processing options
- **Test 10**: Test buffer and handler options

#### **Quick Test Commands**
The script provides ready-to-use commands for testing:
```bash
# Check default command
caget TPX3:COMMAND_LINE

# Enable HTTP logging
caput TPX3:HTTP_LOG_ENABLE 1
caput TPX3:HTTP_LOG "/tmp/serval.log"

# Disable HTTP port
caput TPX3:HTTP_PORT_ENABLE 0

# Enable pipeline options
caput TPX3:UDP_RECEIVERS_ENABLE 1
caput TPX3:UDP_RECEIVERS 4

# Reset to defaults
caput TPX3:HTTP_PORT_ENABLE 1
caput TPX3:HTTP_LOG_ENABLE 0
```

#### **Usage**
```bash
cd test
./build_test.sh
```

#### **What It Does**
1. **Cleans previous build** - Runs `make clean`
2. **Builds the IOC** - Runs `make`
3. **Verifies success** - Checks exit codes and reports results

#### **Example Output**
```bash
🧪 Testing TPX3 Serval IOC Build...

🧹 Cleaning previous build...
make clean
make[1]: Entering directory '/epics/iocs/serval'
make[1]: Leaving directory '/epics/iocs/serval'
✅ Clean completed successfully

🔨 Building IOC...
make
make[1]: Entering directory '/epics/iocs/serval'
make[1]: Leaving directory '/epics/iocs/serval'
✅ Build completed successfully

🎉 Build test completed successfully!
```

## 🚀 **Running Tests**

### **Prerequisites**
- EPICS base and asyn support properly configured
- Make tools available in PATH
- Sufficient permissions to build in the project directory

### **Quick Test**
```bash
# From the main project directory
cd test
./build_test.sh
```

### **Manual Testing**
If you prefer to run tests manually:
```bash
# Clean build
make clean

# Build IOC
make

# Check results
ls -la bin/linux-x86_64/
```

## 📊 **Test Results**

### **Success Indicators**
- ✅ **Clean completed successfully** - Previous build artifacts removed
- ✅ **Build completed successfully** - IOC binary created without errors
- ✅ **Binary exists** - `tpx3serval` executable in `bin/linux-x86_64/`

### **Functionality Test Success Indicators**
- ✅ **All _ENABLE PVs accessible** - Can read/write all enable flags
- ✅ **Command line updates** - COMMAND_LINE PV reflects enable/disable changes
- ✅ **Real-time response** - Changes take effect immediately
- ✅ **Default behavior correct** - Only default options appear in initial command

### **Failure Indicators**
- ❌ **Clean failed** - Permission or file system issues
- ❌ **Build failed** - Compilation errors, missing dependencies
- ❌ **Binary missing** - Build completed but no executable created

## 🔧 **Troubleshooting**

### **Common Issues**

#### **Permission Denied**
```bash
chmod +x test/build_test.sh
```

#### **Make Not Found**
```bash
# Install make tools
sudo apt-get install build-essential
```

#### **EPICS Not Configured**
```bash
# Source EPICS environment
source /epics/base/setEpicsEnv.sh
```

#### **Dependencies Missing**
```bash
# Check EPICS modules
ls /epics/support/
# Verify asyn support is available
```

### **Debug Mode**
For more detailed output, you can run make with verbose flags:
```bash
make clean V=1
make V=1
```

## 📋 **Adding New Tests**

### **Test Script Guidelines**
1. **Clear naming** - Use descriptive names ending in `.sh`
2. **Error handling** - Check exit codes and report failures
3. **User feedback** - Provide clear success/failure messages
4. **Documentation** - Include usage instructions in script comments

### **Example Test Script Structure**
```bash
#!/bin/bash

echo "🧪 Testing [Feature Name]..."

# Test steps
echo "Step 1: [Description]"
# ... test code ...

if [ $? -eq 0 ]; then
    echo "✅ [Step 1] completed successfully"
else
    echo "❌ [Step 1] failed"
    exit 1
fi

echo "🎉 [Feature] test completed successfully!"
```

## 🔍 **Integration with CI/CD**

These test scripts can be integrated into:
- **GitHub Actions** - Automated testing on commits
- **Jenkins** - Continuous integration pipelines
- **Local development** - Pre-commit testing
- **Documentation** - Build verification for releases

## 📚 **Related Documentation**

- **[Main Project](../README.md)** - Project overview and quick start
- **[Build Verification](../docs/BUILD_SUCCESS.md)** - Detailed build testing results
- **[Configuration](../docs/CONFIGURATION.md)** - Build configuration options
- **[Usage Examples](../docs/EXAMPLE_USAGE.md)** - Testing scenarios
- **[Enable Functionality](../docs/ENABLE_FUNCTIONALITY_IMPLEMENTATION.md)** - Complete enable/disable functionality documentation

---

**Note**: Always run tests in a clean environment to ensure reliable results. The build test script is designed to be simple and reliable for daily development use.

