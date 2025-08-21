TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = tpx3serval
# tpx3serval.dbd will be created and installed
DBD += tpx3serval.dbd

# tpx3serval.dbd will be made up from these files:
tpx3serval_DBD += base.dbd

# Include dbd files from all support applications:
#tpx3serval_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#tpx3serval_LIBS += xxx

# tpx3serval_registerRecordDeviceDriver.cpp derives from tpx3serval.dbd
tpx3serval_SRCS += tpx3serval_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
tpx3serval_SRCS_DEFAULT += tpx3servalMain.cpp
tpx3serval_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#tpx3serval_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
tpx3serval_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

