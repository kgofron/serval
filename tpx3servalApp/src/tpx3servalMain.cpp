/* tpx3servalMain.cpp */
/* Author:  Marty Kraimer Date:    17MAR2000 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#include "epicsExit.h"
#include "epicsThread.h"
#include "iocsh.h"
#include "tpx3servalDriver.h"
#include "epicsExit.h"

// Cleanup function for driver
static void cleanupDriver()
{
    extern void performTpx3servalCleanup();
    performTpx3servalCleanup();
}

// Wrapper function for epicsAtExit (matches expected signature)
static void cleanupDriverWrapper(void* arg)
{
    (void)arg;  // Unused parameter
    cleanupDriver();
}

// Signal handler for IOC shutdown
static void signalHandler(int sig)
{
    printf("Received signal %d, shutting down IOC...\n", sig);
    
    // Clean up driver if it exists
    cleanupDriver();
    
    // Exit IOC
    epicsExit(0);
}

int main(int argc,char *argv[])
{
    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);   // Ctrl+C
    signal(SIGTERM, signalHandler);  // Termination request
    signal(SIGQUIT, signalHandler);  // Quit request
    
    // Register cleanup function to be called at exit
    atexit(cleanupDriver);
    
    // Also register with EPICS exit handlers for additional safety
    epicsAtExit(cleanupDriverWrapper, NULL);
    
    if(argc>=2) {
        iocsh(argv[1]);
        epicsThreadSleep(.2);
    }
    iocsh(NULL);
    epicsExit(0);
    return(0);
}
