#ifndef tpx3servalDriver_H
#define tpx3servalDriver_H

#include <asynPortDriver.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsThread.h>
#include <string>

#define MAX_COMMAND_LENGTH 2048
#define MAX_ERROR_LENGTH 256
#define NUM_PARAMS 50

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
    int httpPortIndex_;
    int spidrNetIndex_;
    int tcpIpIndex_;
    int tcpPortIndex_;
    int deviceMaskIndex_;
    int udpReceiversIndex_;
    int frameAssemblersIndex_;
    int ringBufferSizeIndex_;
    int networkBufferSizeIndex_;
    int fileWritersIndex_;
    int correctionHandlersIndex_;
    int processingHandlersIndex_;
    int resourcePoolSizeIndex_;
    int imagePoolSizeIndex_;
    int integrationPoolSizeIndex_;
    int tcpDebugIndex_;
    int releaseResourcesIndex_;
    int experimentalIndex_;
    int jarFileNameIndex_;
    int jarFilePathIndex_;
    int jarFileRbvIndex_;
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
    int httpPort_;
    std::string spidrNet_;
    std::string tcpIp_;
    int tcpPort_;
    int deviceMask_;
    int udpReceivers_;
    int frameAssemblers_;
    int ringBufferSize_;
    int networkBufferSize_;
    int fileWriters_;
    int correctionHandlers_;
    int processingHandlers_;
    int resourcePoolSize_;
    int imagePoolSize_;
    int integrationPoolSize_;
    std::string tcpDebug_;
    bool releaseResources_;
    bool experimental_;
    std::string jarFileName_;
    std::string jarFilePath_;

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
