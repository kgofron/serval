#ifndef tpx3servalDriver_H
#define tpx3servalDriver_H

#include <asynPortDriver.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsThread.h>
#include <string>

#define MAX_COMMAND_LENGTH 2048
#define MAX_ERROR_LENGTH 256
#define NUM_PARAMS 75

class tpx3servalDriver : public asynPortDriver {
public:
    tpx3servalDriver(const char *portName, int maxAddr);
    virtual ~tpx3servalDriver();

    // asynPortDriver methods
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);

    // Public cleanup method for external access
    void cleanupAllProcesses();
    
    // Debug method to get process information
    void printProcessInfo();

private:
    // Parameter indices
    int startIndex_;
    int httpLogIndex_;
    int httpLogEnableIndex_;
    int httpPortIndex_;
    int httpPortEnableIndex_;
    int spidrNetIndex_;
    int spidrNetEnableIndex_;
    int tcpIpIndex_;
    int tcpIpEnableIndex_;
    int tcpPortIndex_;
    int tcpPortEnableIndex_;
    int deviceMaskIndex_;
    int deviceMaskEnableIndex_;
    int udpReceiversIndex_;
    int udpReceiversEnableIndex_;
    int frameAssemblersIndex_;
    int frameAssemblersEnableIndex_;
    int ringBufferSizeIndex_;
    int ringBufferSizeEnableIndex_;
    int networkBufferSizeIndex_;
    int networkBufferSizeEnableIndex_;
    int fileWritersIndex_;
    int fileWritersEnableIndex_;
    int correctionHandlersIndex_;
    int correctionHandlersEnableIndex_;
    int processingHandlersIndex_;
    int processingHandlersEnableIndex_;
    int resourcePoolSizeIndex_;
    int resourcePoolSizeEnableIndex_;
    int imagePoolSizeIndex_;
    int imagePoolSizeEnableIndex_;
    int integrationPoolSizeIndex_;
    int integrationPoolSizeEnableIndex_;
    int tcpDebugIndex_;
    int tcpDebugEnableIndex_;
    int releaseResourcesIndex_;
    int experimentalIndex_;
    int jarFileNameIndex_;
    int jarFilePathIndex_;
    int jarFileRbvIndex_;
    int jarFileEnableIndex_;
    int statusIndex_;
    int processIdIndex_;
    int commandLineIndex_;
    int errorMsgIndex_;

    // Process management
    pid_t processId_;
    bool isRunning_;
    std::string lastError_;
    std::string processCommandLine_;  // Store the exact command line for precise cleanup
    epicsMutexId mutex_;
    epicsEventId stopEvent_;
    epicsThreadId monitorThreadId_;

    // Configuration
    std::string httpLog_;
    bool httpLogEnable_;
    int httpPort_;
    bool httpPortEnable_;
    std::string spidrNet_;
    bool spidrNetEnable_;
    std::string tcpIp_;
    bool tcpIpEnable_;
    int tcpPort_;
    bool tcpPortEnable_;
    int deviceMask_;
    bool deviceMaskEnable_;
    int udpReceivers_;
    bool udpReceiversEnable_;
    int frameAssemblers_;
    bool frameAssemblersEnable_;
    int ringBufferSize_;
    bool ringBufferSizeEnable_;
    int networkBufferSize_;
    bool networkBufferSizeEnable_;
    int fileWriters_;
    bool fileWritersEnable_;
    int correctionHandlers_;
    bool correctionHandlersEnable_;
    int processingHandlers_;
    bool processingHandlersEnable_;
    int resourcePoolSize_;
    bool resourcePoolSizeEnable_;
    int imagePoolSize_;
    bool imagePoolSizeEnable_;
    int integrationPoolSize_;
    bool integrationPoolSizeEnable_;
    std::string tcpDebug_;
    bool tcpDebugEnable_;
    bool releaseResources_;
    bool experimental_;
    std::string jarFileName_;
    std::string jarFilePath_;
    bool jarFileEnable_;

    // Methods
    void buildCommandString(char *command, size_t maxLen);
    asynStatus startProcess();
    asynStatus stopProcess();
    void forceKillAllProcesses();
    void killAllJavaProcesses();
    void monitorProcess();
    static void monitorThreadC(void *pPvt);
    void updateStatus();
    void setError(const char *errorMsg);
    void updateFileRbvs();
};

// Function to get driver instance for cleanup
extern "C" tpx3servalDriver* getTpx3servalDriver();

// Function to perform cleanup (prevents double cleanup)
extern "C" void performTpx3servalCleanup();

#endif // tpx3servalDriver_H
