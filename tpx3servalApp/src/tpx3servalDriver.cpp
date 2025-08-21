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
    createParam("HTTP_PORT", asynParamInt32, &httpPortIndex_);
    createParam("SPIDR_NET", asynParamOctet, &spidrNetIndex_);
    createParam("TCP_IP", asynParamOctet, &tcpIpIndex_);
    createParam("TCP_PORT", asynParamInt32, &tcpPortIndex_);
    createParam("DEVICE_MASK", asynParamInt32, &deviceMaskIndex_);
    createParam("UDP_RECEIVERS", asynParamInt32, &udpReceiversIndex_);
    createParam("FRAME_ASSEMBLERS", asynParamInt32, &frameAssemblersIndex_);
    createParam("RING_BUFFER_SIZE", asynParamInt32, &ringBufferSizeIndex_);
    createParam("NETWORK_BUFFER_SIZE", asynParamInt32, &networkBufferSizeIndex_);
    createParam("FILE_WRITERS", asynParamInt32, &fileWritersIndex_);
    createParam("CORRECTION_HANDLERS", asynParamInt32, &correctionHandlersIndex_);
    createParam("PROCESSING_HANDLERS", asynParamInt32, &processingHandlersIndex_);
    createParam("RESOURCE_POOL_SIZE", asynParamInt32, &resourcePoolSizeIndex_);
    createParam("IMAGE_POOL_SIZE", asynParamInt32, &imagePoolSizeIndex_);
    createParam("INTEGRATION_POOL_SIZE", asynParamInt32, &integrationPoolSizeIndex_);
    createParam("TCP_DEBUG", asynParamOctet, &tcpDebugIndex_);
    createParam("RELEASE_RESOURCES", asynParamInt32, &releaseResourcesIndex_);
    createParam("EXPERIMENTAL", asynParamInt32, &experimentalIndex_);
    createParam("JarFileName", asynParamOctet, &jarFileNameIndex_);
    createParam("JarFilePath", asynParamOctet, &jarFilePathIndex_);
    createParam("JarFile_RBV", asynParamOctet, &jarFileRbvIndex_);
    createParam("STATUS", asynParamInt32, &statusIndex_);
    createParam("PROCESS_ID", asynParamOctet, &processIdIndex_);
    createParam("COMMAND_LINE", asynParamOctet, &commandLineIndex_);
    createParam("ERROR_MSG", asynParamOctet, &errorMsgIndex_);

    // Initialize configuration with default values
    httpLog_ = "";
    httpPort_ = 8081;  // Default as specified in requirements
    spidrNet_ = "autodiscover";
    tcpIp_ = "autodiscover";
    tcpPort_ = 50000;
    deviceMask_ = 0;  // unset
    udpReceivers_ = 0;  // autotuning
    frameAssemblers_ = 0;  // autotuning
    ringBufferSize_ = 0;  // autotuning
    networkBufferSize_ = 0;  // autotuning
    fileWriters_ = 0;  // autotuning
    correctionHandlers_ = 0;  // autotuning
    processingHandlers_ = 0;  // autotuning
    resourcePoolSize_ = 524288;  // Default as specified in requirements
    imagePoolSize_ = 0;  // autotuning
    integrationPoolSize_ = 0;  // autotuning
    tcpDebug_ = "";
    releaseResources_ = false;
    experimental_ = false;
    jarFileName_ = "serval-4.1.1-rc1.jar";
    jarFilePath_ = "../../ASI";

    // Set initial values
    setIntegerParam(statusIndex_, 0);
    setIntegerParam(startIndex_, 0);
    setStringParam(processIdIndex_, "0");
    setStringParam(commandLineIndex_, "");
    setStringParam(errorMsgIndex_, "IOC initialized successfully");
    
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
    } else if (function == tcpPortIndex_) {
        tcpPort_ = value;
    } else if (function == deviceMaskIndex_) {
        deviceMask_ = value;
    } else if (function == udpReceiversIndex_) {
        udpReceivers_ = value;
    } else if (function == frameAssemblersIndex_) {
        frameAssemblers_ = value;
    } else if (function == ringBufferSizeIndex_) {
        ringBufferSize_ = value;
    } else if (function == networkBufferSizeIndex_) {
        networkBufferSize_ = value;
    } else if (function == fileWritersIndex_) {
        fileWriters_ = value;
    } else if (function == correctionHandlersIndex_) {
        correctionHandlers_ = value;
    } else if (function == processingHandlersIndex_) {
        processingHandlers_ = value;
    } else if (function == resourcePoolSizeIndex_) {
        resourcePoolSize_ = value;
    } else if (function == imagePoolSizeIndex_) {
        imagePoolSize_ = value;
    } else if (function == integrationPoolSizeIndex_) {
        integrationPoolSize_ = value;
    } else if (function == releaseResourcesIndex_) {
        releaseResources_ = (value != 0);
    } else if (function == experimentalIndex_) {
        experimental_ = (value != 0);
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

    // Add jar file path
    std::string fullJarPath = jarFilePath_;
    if (!fullJarPath.empty() && fullJarPath.back() != '/' && fullJarPath.back() != '\\') {
        fullJarPath += "/";
    }
    fullJarPath += jarFileName_;
    
    int newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " %s", fullJarPath.c_str());
    if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
        len += static_cast<size_t>(newLen);
    }

    // Add HTTP port (always included as per requirements)
    newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --httpPort=%d", httpPort_);
    if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
        len += static_cast<size_t>(newLen);
    }

    // Add resource pool size (always included as per requirements)
    newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --resourcePoolSize=%d", resourcePoolSize_);
    if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
        len += static_cast<size_t>(newLen);
    }

    // Add optional parameters if they are set
    if (!httpLog_.empty()) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --httpLog=%s", httpLog_.c_str());
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (spidrNet_ != "autodiscover") {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --spidrNet=%s", spidrNet_.c_str());
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (tcpIp_ != "autodiscover") {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --tcpIp=%s", tcpIp_.c_str());
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (tcpPort_ != 50000) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --tcpPort=%d", tcpPort_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (deviceMask_ != 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --deviceMask=%d", deviceMask_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (udpReceivers_ > 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --udpReceivers=%d", udpReceivers_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (frameAssemblers_ > 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --frameAssemblers=%d", frameAssemblers_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (ringBufferSize_ > 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --ringBufferSize=%d", ringBufferSize_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (networkBufferSize_ > 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --networkBufferSize=%d", networkBufferSize_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (fileWriters_ > 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --fileWriters=%d", fileWriters_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (correctionHandlers_ > 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --correctionHandlers=%d", correctionHandlers_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (processingHandlers_ > 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --processingHandlers=%d", processingHandlers_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (imagePoolSize_ > 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --imagePoolSize=%d", imagePoolSize_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (integrationPoolSize_ > 0) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --integrationPoolSize=%d", integrationPoolSize_);
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (!tcpDebug_.empty()) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --tcpDebug=%s", tcpDebug_.c_str());
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (releaseResources_) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --releaseResources");
        if (newLen > 0 && len + static_cast<size_t>(newLen) < sizeof(tempCmd)) {
            len += static_cast<size_t>(newLen);
        }
    }

    if (experimental_) {
        newLen = snprintf(tempCmd + len, sizeof(tempCmd) - len, " --experimental");
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
