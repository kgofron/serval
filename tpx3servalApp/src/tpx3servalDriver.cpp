#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#include "epicsExport.h"
#include "iocsh.h"
#include "tpx3servalDriver.h"

static const char *driverName = "tpx3servalDriver";

// Constructor
tpx3servalDriver::tpx3servalDriver(const char *portName, int maxAddr)
    : asynPortDriver(portName, maxAddr, 
                     NUM_PARAMS,
                     asynInt32Mask | asynOctetMask | asynDrvUserMask,
                     asynInt32Mask | asynOctetMask,
                     ASYN_CANBLOCK, 1, 0, 0),
      processId_(0), isRunning_(false), monitorThreadId_(0)
{
    // Create mutex and event
    mutex_ = epicsMutexCreate();
    stopEvent_ = epicsEventCreate(epicsEventEmpty);

    // Create parameters
    createParam("START", asynParamInt32, &startIndex_);
    createParam("HTTP_LOG", asynParamOctet, &httpLogIndex_);
    createParam("HTTP_LOG_ENABLE", asynParamInt32, &httpLogEnableIndex_);
    createParam("HTTP_PORT", asynParamInt32, &httpPortIndex_);
    createParam("HTTP_PORT_ENABLE", asynParamInt32, &httpPortEnableIndex_);
    createParam("SPIDR_NET", asynParamOctet, &spidrNetIndex_);
    createParam("SPIDR_NET_ENABLE", asynParamInt32, &spidrNetEnableIndex_);
    createParam("TCP_IP", asynParamOctet, &tcpIpIndex_);
    createParam("TCP_IP_ENABLE", asynParamInt32, &tcpIpEnableIndex_);
    createParam("TCP_PORT", asynParamInt32, &tcpPortIndex_);
    createParam("TCP_PORT_ENABLE", asynParamInt32, &tcpPortEnableIndex_);
    createParam("DEVICE_MASK", asynParamInt32, &deviceMaskIndex_);
    createParam("DEVICE_MASK_ENABLE", asynParamInt32, &deviceMaskEnableIndex_);
    createParam("UDP_RECEIVERS", asynParamInt32, &udpReceiversIndex_);
    createParam("UDP_RECEIVERS_ENABLE", asynParamInt32, &udpReceiversEnableIndex_);
    createParam("FRAME_ASSEMBLERS", asynParamInt32, &frameAssemblersIndex_);
    createParam("FRAME_ASSEMBLERS_ENABLE", asynParamInt32, &frameAssemblersEnableIndex_);
    createParam("RING_BUFFER_SIZE", asynParamInt32, &ringBufferSizeIndex_);
    createParam("RING_BUFFER_SIZE_ENABLE", asynParamInt32, &ringBufferSizeEnableIndex_);
    createParam("NETWORK_BUFFER_SIZE", asynParamInt32, &networkBufferSizeIndex_);
    createParam("NETWORK_BUFFER_SIZE_ENABLE", asynParamInt32, &networkBufferSizeEnableIndex_);
    createParam("FILE_WRITERS", asynParamInt32, &fileWritersIndex_);
    createParam("FILE_WRITERS_ENABLE", asynParamInt32, &fileWritersEnableIndex_);
    createParam("CORRECTION_HANDLERS", asynParamInt32, &correctionHandlersIndex_);
    createParam("CORRECTION_HANDLERS_ENABLE", asynParamInt32, &correctionHandlersEnableIndex_);
    createParam("PROCESSING_HANDLERS", asynParamInt32, &processingHandlersIndex_);
    createParam("PROCESSING_HANDLERS_ENABLE", asynParamInt32, &processingHandlersEnableIndex_);
    createParam("RESOURCE_POOL_SIZE", asynParamInt32, &resourcePoolSizeIndex_);
    createParam("RESOURCE_POOL_SIZE_ENABLE", asynParamInt32, &resourcePoolSizeEnableIndex_);
    createParam("IMAGE_POOL_SIZE", asynParamInt32, &imagePoolSizeIndex_);
    createParam("IMAGE_POOL_SIZE_ENABLE", asynParamInt32, &imagePoolSizeEnableIndex_);
    createParam("INTEGRATION_POOL_SIZE", asynParamInt32, &integrationPoolSizeIndex_);
    createParam("INTEGRATION_POOL_SIZE_ENABLE", asynParamInt32, &integrationPoolSizeEnableIndex_);
    createParam("TCP_DEBUG", asynParamOctet, &tcpDebugIndex_);
    createParam("TCP_DEBUG_ENABLE", asynParamInt32, &tcpDebugEnableIndex_);
    createParam("RELEASE_RESOURCES", asynParamInt32, &releaseResourcesIndex_);
    createParam("EXPERIMENTAL", asynParamInt32, &experimentalIndex_);
    createParam("JarFileName", asynParamOctet, &jarFileNameIndex_);
    createParam("JarFilePath", asynParamOctet, &jarFilePathIndex_);
    createParam("JarFile_RBV", asynParamOctet, &jarFileRbvIndex_);
    createParam("JAR_FILE_ENABLE", asynParamInt32, &jarFileEnableIndex_);
    createParam("STATUS", asynParamInt32, &statusIndex_);
    createParam("PROCESS_ID", asynParamOctet, &processIdIndex_);
    createParam("COMMAND_LINE", asynParamOctet, &commandLineIndex_);
    createParam("ERROR_MSG", asynParamOctet, &errorMsgIndex_);

    // Initialize configuration with default values
    httpLog_ = "";
    httpLogEnable_ = false;  // Default: disabled
    httpPort_ = 8081;  // Default as specified in requirements
    httpPortEnable_ = true;  // Default: enabled
    spidrNet_ = "autodiscover";
    spidrNetEnable_ = false;  // Default: disabled
    tcpIp_ = "autodiscover";
    tcpIpEnable_ = false;  // Default: disabled
    tcpPort_ = 50000;
    tcpPortEnable_ = true;  // Default: enabled
    deviceMask_ = 0;  // unset
    deviceMaskEnable_ = false;  // Default: disabled
    udpReceivers_ = 0;  // autotuning
    udpReceiversEnable_ = false;  // Default: disabled
    frameAssemblers_ = 0;  // autotuning
    frameAssemblersEnable_ = false;  // Default: disabled
    ringBufferSize_ = 0;  // autotuning
    ringBufferSizeEnable_ = false;  // Default: disabled
    networkBufferSize_ = 0;  // autotuning
    networkBufferSizeEnable_ = false;  // Default: disabled
    fileWriters_ = 0;  // autotuning
    fileWritersEnable_ = false;  // Default: disabled
    correctionHandlers_ = 0;  // autotuning
    correctionHandlersEnable_ = false;  // Default: disabled
    processingHandlers_ = 0;  // autotuning
    processingHandlersEnable_ = false;  // Default: disabled
    resourcePoolSize_ = 524288;  // Default as specified in requirements
    resourcePoolSizeEnable_ = true;  // Default: enabled
    imagePoolSize_ = 0;  // autotuning
    imagePoolSizeEnable_ = false;  // Default: disabled
    integrationPoolSize_ = 0;  // autotuning
    integrationPoolSizeEnable_ = false;  // Default: disabled
    tcpDebug_ = "";
    tcpDebugEnable_ = false;  // Default: disabled
    releaseResources_ = false;
    experimental_ = false;
    jarFileName_ = "serval-4.1.1-rc1.jar";
    jarFilePath_ = "../../ASI";
    jarFileEnable_ = true;  // Default: enabled

    // Set initial values
    setIntegerParam(statusIndex_, 0);
    setIntegerParam(startIndex_, 0);
    setIntegerParam(httpLogEnableIndex_, httpLogEnable_ ? 1 : 0);
    setIntegerParam(httpPortEnableIndex_, httpPortEnable_ ? 1 : 0);
    setIntegerParam(spidrNetEnableIndex_, spidrNetEnable_ ? 1 : 0);
    setIntegerParam(tcpIpEnableIndex_, tcpIpEnable_ ? 1 : 0);
    setIntegerParam(tcpPortEnableIndex_, tcpPortEnable_ ? 1 : 0);
    setIntegerParam(deviceMaskEnableIndex_, deviceMaskEnable_ ? 1 : 0);
    setIntegerParam(udpReceiversEnableIndex_, udpReceiversEnable_ ? 1 : 0);
    setIntegerParam(frameAssemblersEnableIndex_, frameAssemblersEnable_ ? 1 : 0);
    setIntegerParam(ringBufferSizeEnableIndex_, ringBufferSizeEnable_ ? 1 : 0);
    setIntegerParam(networkBufferSizeEnableIndex_, networkBufferSizeEnable_ ? 1 : 0);
    setIntegerParam(fileWritersEnableIndex_, fileWritersEnable_ ? 1 : 0);
    setIntegerParam(correctionHandlersEnableIndex_, correctionHandlersEnable_ ? 1 : 0);
    setIntegerParam(processingHandlersEnableIndex_, processingHandlersEnable_ ? 1 : 0);
    setIntegerParam(resourcePoolSizeEnableIndex_, resourcePoolSizeEnable_ ? 1 : 0);
    setIntegerParam(imagePoolSizeEnableIndex_, imagePoolSizeEnable_ ? 1 : 0);
    setIntegerParam(integrationPoolSizeEnableIndex_, integrationPoolSizeEnable_ ? 1 : 0);
    setIntegerParam(tcpDebugEnableIndex_, tcpDebugEnable_ ? 1 : 0);
    setIntegerParam(jarFileEnableIndex_, jarFileEnable_ ? 1 : 0);
    setStringParam(processIdIndex_, "0");
    setStringParam(commandLineIndex_, "");
    setStringParam(errorMsgIndex_, "IOC initialized successfully");
    setStringParam(jarFileNameIndex_, jarFileName_.c_str());
    setStringParam(jarFilePathIndex_, jarFilePath_.c_str());
    
    // Update file RBV parameter with initial combined path
    updateFileRbvs();

    // Start monitor thread
    monitorThreadId_ = epicsThreadCreate("tpx3servalMonitor", 
                                        epicsThreadPriorityMedium,
                                        epicsThreadGetStackSize(epicsThreadStackMedium),
                                        monitorThreadC, this);
    if (!monitorThreadId_) {
        printf("%s:%s: Failed to create monitor thread\n", driverName, __FUNCTION__);
    }

    callParamCallbacks();
}

// Destructor
tpx3servalDriver::~tpx3servalDriver()
{
    printf("%s:%s: Destructor called, cleaning up...\n", driverName, __FUNCTION__);
    
    // Signal monitor thread to stop first
    if (monitorThreadId_) {
        epicsEventSignal(stopEvent_);
        
        // Wait for monitor thread with timeout to avoid hanging
        epicsThreadId threadId = monitorThreadId_;
        monitorThreadId_ = 0;  // Clear the ID first
        
        // Try to join with timeout
        if (epicsThreadIsSuspended(threadId)) {
            epicsThreadResume(threadId);
        }
        
        // Use a non-blocking approach
        epicsThreadSleep(0.1);  // Give thread time to exit
        
        printf("%s:%s: Monitor thread cleanup completed\n", driverName, __FUNCTION__);
    }
    
    // Force kill any running processes after thread cleanup
    if (isRunning_) {
        forceKillAllProcesses();
    }
    
    // Destroy resources in reverse order of creation
    if (stopEvent_) {
        epicsEventDestroy(stopEvent_);
    }
    if (mutex_) {
        epicsMutexDestroy(mutex_);
    }
    
    printf("%s:%s: Destructor cleanup completed\n", driverName, __FUNCTION__);
}

// Write int32 parameter
asynStatus tpx3servalDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;

    status = setIntegerParam(function, value);

    if (function == startIndex_) {
        if (value == 1 && !isRunning_) {
            // Start the process if not already running
            status = startProcess();
            if (status == asynSuccess) {
                isRunning_ = true;
                setIntegerParam(startIndex_, 1);
            } else {
                setStringParam(errorMsgIndex_, "Failed to start process - check configuration");
            }
        } else if (value == 0 && isRunning_) {
            // Stop the process if currently running
            status = stopProcess();
            if (status == asynSuccess) {
                isRunning_ = false;
                setIntegerParam(startIndex_, 0);
            } else {
                setStringParam(errorMsgIndex_, "Failed to stop process");
            }
        } else if (value == 1 && isRunning_) {
            // Already running, ignore start request
            setStringParam(errorMsgIndex_, "Process already running - start request ignored");
        } else if (value == 0 && !isRunning_) {
            // Already stopped, ignore stop request
            setStringParam(errorMsgIndex_, "Process already stopped - stop request ignored");
        }
    } else if (function == httpPortIndex_) {
        httpPort_ = value;
        setStringParam(errorMsgIndex_, "HTTP port updated successfully");
    } else if (function == httpPortEnableIndex_) {
        httpPortEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "HTTP port enabled" : "HTTP port disabled");
    } else if (function == tcpPortIndex_) {
        tcpPort_ = value;
        setStringParam(errorMsgIndex_, "TCP port updated successfully");
    } else if (function == tcpPortEnableIndex_) {
        tcpPortEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "TCP port enabled" : "TCP port disabled");
    } else if (function == deviceMaskIndex_) {
        deviceMask_ = value;
        setStringParam(errorMsgIndex_, "Device mask updated successfully");
    } else if (function == deviceMaskEnableIndex_) {
        deviceMaskEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Device mask enabled" : "Device mask disabled");
    } else if (function == udpReceiversIndex_) {
        udpReceivers_ = value;
        setStringParam(errorMsgIndex_, "UDP receivers updated successfully");
    } else if (function == udpReceiversEnableIndex_) {
        udpReceiversEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "UDP receivers enabled" : "UDP receivers disabled");
    } else if (function == frameAssemblersIndex_) {
        frameAssemblers_ = value;
        setStringParam(errorMsgIndex_, "Frame assemblers updated successfully");
    } else if (function == frameAssemblersEnableIndex_) {
        frameAssemblersEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Frame assemblers enabled" : "Frame assemblers disabled");
    } else if (function == ringBufferSizeIndex_) {
        ringBufferSize_ = value;
        setStringParam(errorMsgIndex_, "Ring buffer size updated successfully");
    } else if (function == ringBufferSizeEnableIndex_) {
        ringBufferSizeEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Ring buffer size enabled" : "Ring buffer size disabled");
    } else if (function == networkBufferSizeIndex_) {
        networkBufferSize_ = value;
        setStringParam(errorMsgIndex_, "Network buffer size updated successfully");
    } else if (function == networkBufferSizeEnableIndex_) {
        networkBufferSizeEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Network buffer size enabled" : "Network buffer size disabled");
    } else if (function == fileWritersIndex_) {
        fileWriters_ = value;
        setStringParam(errorMsgIndex_, "File writers updated successfully");
    } else if (function == fileWritersEnableIndex_) {
        fileWritersEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "File writers enabled" : "File writers disabled");
    } else if (function == correctionHandlersIndex_) {
        correctionHandlers_ = value;
        setStringParam(errorMsgIndex_, "Correction handlers updated successfully");
    } else if (function == correctionHandlersEnableIndex_) {
        correctionHandlersEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Correction handlers enabled" : "Correction handlers disabled");
    } else if (function == processingHandlersIndex_) {
        processingHandlers_ = value;
        setStringParam(errorMsgIndex_, "Processing handlers updated successfully");
    } else if (function == processingHandlersEnableIndex_) {
        processingHandlersEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Processing handlers enabled" : "Processing handlers disabled");
    } else if (function == resourcePoolSizeIndex_) {
        resourcePoolSize_ = value;
        setStringParam(errorMsgIndex_, "Resource pool size updated successfully");
    } else if (function == resourcePoolSizeEnableIndex_) {
        resourcePoolSizeEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Resource pool size enabled" : "Resource pool size disabled");
    } else if (function == imagePoolSizeIndex_) {
        imagePoolSize_ = value;
        setStringParam(errorMsgIndex_, "Image pool size updated successfully");
    } else if (function == imagePoolSizeEnableIndex_) {
        imagePoolSizeEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Image pool size enabled" : "Image pool size disabled");
    } else if (function == integrationPoolSizeIndex_) {
        integrationPoolSize_ = value;
        setStringParam(errorMsgIndex_, "Integration pool size updated successfully");
    } else if (function == integrationPoolSizeEnableIndex_) {
        integrationPoolSizeEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Integration pool size enabled" : "Integration pool size disabled");
    } else if (function == releaseResourcesIndex_) {
        releaseResources_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Release resources enabled" : "Release resources disabled");
    } else if (function == experimentalIndex_) {
        experimental_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "Experimental mode enabled" : "Experimental mode disabled");
    } else if (function == httpLogEnableIndex_) {
        httpLogEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "HTTP logging enabled" : "HTTP logging disabled");
    } else if (function == spidrNetEnableIndex_) {
        spidrNetEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "SPIDR net enabled" : "SPIDR net disabled");
    } else if (function == tcpIpEnableIndex_) {
        tcpIpEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "TCP IP enabled" : "TCP IP disabled");
    } else if (function == tcpDebugEnableIndex_) {
        tcpDebugEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "TCP debug enabled" : "TCP debug disabled");
    } else if (function == jarFileEnableIndex_) {
        jarFileEnable_ = (value != 0);
        setStringParam(errorMsgIndex_, value ? "JAR file enabled" : "JAR file disabled");
    }

    callParamCallbacks();
    return status;
}

// Write octet parameter
asynStatus tpx3servalDriver::writeOctet(asynUser *pasynUser, const char *value, 
                                         size_t maxChars, size_t *nActual)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;

    *nActual = maxChars;

    if (function == httpLogIndex_) {
        httpLog_ = std::string(value, maxChars);
        setStringParam(errorMsgIndex_, "HTTP log path updated successfully");
    } else if (function == spidrNetIndex_) {
        spidrNet_ = std::string(value, maxChars);
        setStringParam(errorMsgIndex_, "SPIDR net updated successfully");
    } else if (function == tcpIpIndex_) {
        tcpIp_ = std::string(value, maxChars);
        setStringParam(errorMsgIndex_, "TCP IP updated successfully");
    } else if (function == tcpDebugIndex_) {
        tcpDebug_ = std::string(value, maxChars);
        setStringParam(errorMsgIndex_, "TCP debug path updated successfully");
    } else if (function == jarFileNameIndex_) {
        if (strlen(value) == 0) {
            setStringParam(errorMsgIndex_, "JAR filename cannot be empty");
        } else {
            jarFileName_ = std::string(value, maxChars);
            updateFileRbvs();
            setStringParam(errorMsgIndex_, "JAR filename updated successfully");
        }
    } else if (function == jarFilePathIndex_) {
        if (strlen(value) == 0) {
            setStringParam(errorMsgIndex_, "JAR file path cannot be empty");
        } else {
            jarFilePath_ = std::string(value, maxChars);
            updateFileRbvs();
            setStringParam(errorMsgIndex_, "JAR file path updated successfully");
        }
    }

    status = setStringParam(function, value);
    callParamCallbacks();
    return status;
}

// Build command string
void tpx3servalDriver::buildCommandString(char *command, size_t maxLen)
{
    char tempCmd[MAX_COMMAND_LENGTH];
    size_t len = 0;

    // Start with java command
    len = snprintf(tempCmd, sizeof(tempCmd), "java -jar");
    if (len >= sizeof(tempCmd)) {
        strncpy(command, "java -jar", maxLen - 1);
        command[maxLen - 1] = '\0';
        return;
    }

    // Add jar file path if enabled
    if (jarFileEnable_) {
        std::string fullJarPath = jarFilePath_;
        if (!fullJarPath.empty() && fullJarPath.back() != '/' && fullJarPath.back() != '\\') {
            fullJarPath += "/";
        }
        fullJarPath += jarFileName_;
        
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " %s", fullJarPath.c_str());
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add HTTP port if enabled
    if (httpPortEnable_) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --httpPort=%d", httpPort_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add resource pool size if enabled
    if (resourcePoolSizeEnable_) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --resourcePoolSize=%d", resourcePoolSize_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add HTTP log if enabled and not empty
    if (httpLogEnable_ && !httpLog_.empty()) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --httpLog=%s", httpLog_.c_str());
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add SPIDR net if enabled and not autodiscover
    if (spidrNetEnable_ && spidrNet_ != "autodiscover") {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --spidrNet=%s", spidrNet_.c_str());
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add TCP IP if enabled and not autodiscover
    if (tcpIpEnable_ && tcpIp_ != "autodiscover") {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --tcpIp=%s", tcpIp_.c_str());
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add TCP port if enabled and not default
    if (tcpPortEnable_ && tcpPort_ != 50000) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --tcpPort=%d", tcpPort_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add device mask if enabled and not 0
    if (deviceMaskEnable_ && deviceMask_ != 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --deviceMask=%d", deviceMask_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add UDP receivers if enabled and greater than 0
    if (udpReceiversEnable_ && udpReceivers_ > 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --udpReceivers=%d", udpReceivers_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add frame assemblers if enabled and greater than 0
    if (frameAssemblersEnable_ && frameAssemblers_ > 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --frameAssemblers=%d", frameAssemblers_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add ring buffer size if enabled and greater than 0
    if (ringBufferSizeEnable_ && ringBufferSize_ > 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --ringBufferSize=%d", ringBufferSize_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add network buffer size if enabled and greater than 0
    if (networkBufferSizeEnable_ && networkBufferSize_ > 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --networkBufferSize=%d", networkBufferSize_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add file writers if enabled and greater than 0
    if (fileWritersEnable_ && fileWriters_ > 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --fileWriters=%d", fileWriters_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add correction handlers if enabled and greater than 0
    if (correctionHandlersEnable_ && correctionHandlers_ > 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --correctionHandlers=%d", correctionHandlers_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add processing handlers if enabled and greater than 0
    if (processingHandlersEnable_ && processingHandlers_ > 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --processingHandlers=%d", processingHandlers_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add image pool size if enabled and greater than 0
    if (imagePoolSizeEnable_ && imagePoolSize_ > 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --imagePoolSize=%d", imagePoolSize_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add integration pool size if enabled and greater than 0
    if (integrationPoolSizeEnable_ && integrationPoolSize_ > 0) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --integrationPoolSize=%d", integrationPoolSize_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add TCP debug if enabled and not empty
    if (tcpDebugEnable_ && !tcpDebug_.empty()) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --tcpDebug=%s", tcpDebug_.c_str());
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add release resources if enabled
    if (releaseResources_) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --releaseResources");
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Add experimental if enabled
    if (experimental_) {
        int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --experimental");
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    // Copy to output buffer
    strncpy(command, tempCmd, maxLen - 1);
    command[maxLen - 1] = '\0';
}

// Start process
asynStatus tpx3servalDriver::startProcess()
{
    epicsMutexLock(mutex_);
    
    if (isRunning_) {
        epicsMutexUnlock(mutex_);
        return asynError;
    }

    char command[MAX_COMMAND_LENGTH];
    buildCommandString(command, sizeof(command));

    // Fork and exec
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl("/bin/bash", "bash", "-c", command, NULL);
        exit(1);
    } else if (pid > 0) {
        // Parent process
        processId_ = pid;
        isRunning_ = true;
        processCommandLine_ = std::string(command);
        setIntegerParam(statusIndex_, 1);
        setIntegerParam(startIndex_, 1);
        setStringParam(processIdIndex_, std::to_string(pid).c_str());
        setStringParam(commandLineIndex_, command);
        setStringParam(errorMsgIndex_, "Process started successfully");
        printf("%s:%s: Started process %d with command: %s\n", 
               driverName, __FUNCTION__, pid, command);
    } else {
        // Fork failed
        setError("Failed to fork process - system resource limit reached");
        epicsMutexUnlock(mutex_);
        return asynError;
    }

    epicsMutexUnlock(mutex_);
    return asynSuccess;
}

// Stop process
asynStatus tpx3servalDriver::stopProcess()
{
    epicsMutexLock(mutex_);
    
    if (!isRunning_) {
        epicsMutexUnlock(mutex_);
        return asynSuccess;
    }

    if (processId_ > 0) {
        // Send SIGTERM first
        if (kill(processId_, SIGTERM) == 0) {
            printf("%s:%s: Sent SIGTERM to process %d\n", driverName, __FUNCTION__, processId_);
            setStringParam(errorMsgIndex_, "Sending SIGTERM to process");
            
            // Wait a bit for graceful shutdown
            epicsThreadSleep(2.0);
            
            // Check if process is still running
            int status;
            if (waitpid(processId_, &status, WNOHANG) == 0) {
                // Process still running, send SIGKILL
                if (kill(processId_, SIGKILL) == 0) {
                    printf("%s:%s: Sent SIGKILL to process %d\n", driverName, __FUNCTION__, processId_);
                    setStringParam(errorMsgIndex_, "Sending SIGKILL to process");
                    waitpid(processId_, &status, 0);
                }
            }
        }
    }

    processId_ = 0;
    isRunning_ = false;
    processCommandLine_.clear();
    setIntegerParam(statusIndex_, 0);
    setIntegerParam(startIndex_, 0);
    setStringParam(processIdIndex_, "0");
    setStringParam(errorMsgIndex_, "Process stopped successfully");

    epicsMutexUnlock(mutex_);
    return asynSuccess;
}

// Force kill all child processes (for emergency cleanup)
void tpx3servalDriver::forceKillAllProcesses()
{
    epicsMutexLock(mutex_);
    
    if (processId_ > 0) {
        printf("%s:%s: Force killing process %d\n", driverName, __FUNCTION__, processId_);
        
        // Send SIGKILL immediately
        if (kill(processId_, SIGKILL) == 0) {
            int status;
            waitpid(processId_, &status, 0);
            printf("%s:%s: Process %d killed\n", driverName, __FUNCTION__, processId_);
        }
        
        processId_ = 0;
        isRunning_ = false;
        processCommandLine_.clear();
        setIntegerParam(statusIndex_, 0);
        setIntegerParam(startIndex_, 0);
        setStringParam(processIdIndex_, "0");
        setStringParam(errorMsgIndex_, "Process force killed");
    }
    
    epicsMutexUnlock(mutex_);
}

// Kill only the specific Java processes started by this IOC (emergency cleanup)
void tpx3servalDriver::killAllJavaProcesses()
{
    printf("%s:%s: Killing only Serval Java processes for cleanup\n", driverName, __FUNCTION__);
    
    // First, try to kill processes using the exact command line we started
    if (!processCommandLine_.empty()) {
        char cmd[512];
        // Escape special characters in the command line for shell safety
        std::string escapedCmd = processCommandLine_;
        // Replace single quotes with '\'' for shell escaping
        size_t pos = 0;
        while ((pos = escapedCmd.find("'", pos)) != std::string::npos) {
            escapedCmd.replace(pos, 1, "'\\''");
            pos += 4;
        }
        
        snprintf(cmd, sizeof(cmd), "pkill -f '^%s$'", escapedCmd.c_str());
        printf("%s:%s: Attempting to kill processes with exact command: %s\n", 
               driverName, __FUNCTION__, escapedCmd.c_str());
        
        int result = system(cmd);
        if (result == 0) {
            printf("%s:%s: Processes with exact command killed\n", driverName, __FUNCTION__);
            setStringParam(errorMsgIndex_, "Processes with exact command killed for cleanup");
            return;
        }
    }
    
    // Fallback: kill only processes containing our specific jar file names
    std::string jarName = jarFileName_;
    if (jarName.find("serval") != std::string::npos) {
        
        // Kill only processes containing our specific jar names
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "pkill -f 'java.*%s'", jarName.c_str());
        printf("%s:%s: Attempting to kill processes with jar name: %s\n", 
               driverName, __FUNCTION__, jarName.c_str());
        
        int result = system(cmd);
        
        if (result == 0) {
            printf("%s:%s: Serval Java processes killed\n", driverName, __FUNCTION__);
            setStringParam(errorMsgIndex_, "Serval Java processes killed for cleanup");
        } else {
            printf("%s:%s: No Serval Java processes found\n", driverName, __FUNCTION__);
            setStringParam(errorMsgIndex_, "No Serval Java processes found for cleanup");
        }
    } else {
        printf("%s:%s: No specific jar file configured, skipping Java process cleanup\n", driverName, __FUNCTION__);
        setStringParam(errorMsgIndex_, "No specific jar file configured for cleanup");
    }
}

// Public cleanup method that can be called externally
void tpx3servalDriver::cleanupAllProcesses()
{
    printf("%s:%s: Public cleanup method called\n", driverName, __FUNCTION__);
    
    // Check if driver is still valid
    if (!mutex_) {
        printf("%s:%s: Driver already destroyed, skipping cleanup\n", driverName, __FUNCTION__);
        return;
    }
    
    // Force kill any running processes
    forceKillAllProcesses();
    
    // Also kill any orphaned Java processes
    killAllJavaProcesses();
}

// Debug method to print current process information
void tpx3servalDriver::printProcessInfo()
{
    printf("%s:%s: Process Information:\n", driverName, __FUNCTION__);
    printf("  Process ID: %d\n", processId_);
    printf("  Is Running: %s\n", isRunning_ ? "Yes" : "No");
    printf("  Command Line: %s\n", processCommandLine_.c_str());
    printf("  Jar File: %s\n", jarFileName_.c_str());
    printf("  Jar Path: %s\n", jarFilePath_.c_str());
    printf("  HTTP Port: %d\n", httpPort_);
    printf("  Resource Pool Size: %d\n", resourcePoolSize_);
}

// Monitor process
void tpx3servalDriver::monitorProcess()
{
    printf("%s:%s: Monitor thread started\n", driverName, __FUNCTION__);
    
    while (true) {
        if (epicsEventWaitWithTimeout(stopEvent_, 1.0) == epicsEventWaitOK) {
            printf("%s:%s: Monitor thread received stop signal\n", driverName, __FUNCTION__);
            break;
        }

        epicsMutexLock(mutex_);
        if (isRunning_ && processId_ > 0) {
            int status;
            pid_t result = waitpid(processId_, &status, WNOHANG);
            
            if (result == processId_) {
                // Process has terminated
                printf("%s:%s: Process %d terminated with status %d\n", 
                       driverName, __FUNCTION__, processId_, status);
                processId_ = 0;
                isRunning_ = false;
                processCommandLine_.clear();
                setIntegerParam(statusIndex_, 0);
                setIntegerParam(startIndex_, 0);
                setStringParam(processIdIndex_, "0");
                if (WIFEXITED(status)) {
                    int exitCode = WEXITSTATUS(status);
                    char exitMsg[100];
                    snprintf(exitMsg, sizeof(exitMsg), "Process exited with code %d", exitCode);
                    setStringParam(errorMsgIndex_, exitMsg);
                } else if (WIFSIGNALED(status)) {
                    int signalNum = WTERMSIG(status);
                    char signalMsg[100];
                    snprintf(signalMsg, sizeof(signalMsg), "Process terminated by signal %d", signalNum);
                    setStringParam(errorMsgIndex_, signalMsg);
                }
            } else if (result == -1 && errno == ECHILD) {
                // Process not found
                processId_ = 0;
                isRunning_ = false;
                processCommandLine_.clear();
                setIntegerParam(statusIndex_, 0);
                setIntegerParam(startIndex_, 0);
                setStringParam(processIdIndex_, "0");
                setStringParam(errorMsgIndex_, "Process not found - may have been killed externally");
            }
        }
        epicsMutexUnlock(mutex_);
        
        callParamCallbacks();
    }
    
    printf("%s:%s: Monitor thread exiting\n", driverName, __FUNCTION__);
}

// Monitor thread C function
void tpx3servalDriver::monitorThreadC(void *pPvt)
{
    tpx3servalDriver *pPvt_ = (tpx3servalDriver*)pPvt;
    pPvt_->monitorProcess();
}

// Set error message
void tpx3servalDriver::setError(const char *errorMsg)
{
    setStringParam(errorMsgIndex_, errorMsg);
    printf("%s:%s: Error: %s\n", driverName, __FUNCTION__, errorMsg);
}

// Update file path RBV parameter with combined path and filename
void tpx3servalDriver::updateFileRbvs()
{
    // Helper function to combine path and filename
    auto combinePathAndFile = [](const std::string& path, const std::string& filename) -> std::string {
        std::string fullPath = path;
        if (!fullPath.empty() && fullPath.back() != '/' && fullPath.back() != '\\') {
            fullPath += "/";
        }
        fullPath += filename;
        return fullPath;
    };
    
    // Update JarFile_RBV
    std::string fullJarPath = combinePathAndFile(jarFilePath_, jarFileName_);
    setStringParam(jarFileRbvIndex_, fullJarPath.c_str());
    
    // Trigger callbacks to update PVs
    callParamCallbacks();
}

// Global driver instance for cleanup
static tpx3servalDriver* g_driver = NULL;
static bool g_cleanup_done = false;

// Export driver
extern "C" {
    int tpx3servalConfigure(const char *portName, int maxAddr)
    {
        // Store the driver instance globally for cleanup during IOC shutdown
        if (g_driver) {
            delete g_driver;  // Clean up any existing instance
        }
        g_driver = new tpx3servalDriver(portName, maxAddr);
        return asynSuccess;
    }
    
    // Function to get driver instance for cleanup
    tpx3servalDriver* getTpx3servalDriver()
    {
        return g_driver;
    }
    
    // Function to perform cleanup (prevents double cleanup)
    void performTpx3servalCleanup()
    {
        if (g_cleanup_done) {
            printf("Cleanup already performed, skipping\n");
            return;
        }
        
        if (g_driver) {
            printf("Performing TPX3 serval driver cleanup...\n");
            g_driver->cleanupAllProcesses();
            delete g_driver;
            g_driver = NULL;
        }
        
        g_cleanup_done = true;
    }
    
    static const iocshArg tpx3servalConfigureArg0 = {"portName", iocshArgString};
    static const iocshArg tpx3servalConfigureArg1 = {"maxAddr", iocshArgInt};
    static const iocshArg * const tpx3servalConfigureArgs[] = {&tpx3servalConfigureArg0, &tpx3servalConfigureArg1};
    static const iocshFuncDef tpx3servalConfigureFuncDef = {"tpx3servalConfigure", 2, tpx3servalConfigureArgs};
    
    static void tpx3servalConfigureCallFunc(const iocshArgBuf *args)
    {
        tpx3servalConfigure(args[0].sval, args[1].ival);
    }
    
    void tpx3servalRegister(void)
    {
        // This function is called by the registrar directive
        // It registers the tpx3servalConfigure function
        iocshRegister(&tpx3servalConfigureFuncDef, tpx3servalConfigureCallFunc);
    }
}

// This is the registrar function that EPICS will call
extern "C" {
    void register_func_tpx3servalConfigure(void)
    {
        tpx3servalRegister();
    }
}

// Export the registrar function
epicsExportRegistrar(register_func_tpx3servalConfigure);
