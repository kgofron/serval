TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

# Add the name of the database to be built
PROD_IOC = tpx3serval

# tpx3serval.dbd will be created and installed
DBD += tpx3serval.dbd

# tpx3serval.dbd will be made up from these files:
tpx3serval_DBD += base.dbd
tpx3serval_DBD += asyn.dbd
tpx3serval_DBD += tpx3servalSupport.dbd

# Remove PyDevice support to avoid linker errors
# tpx3serval_DBD += pydev.dbd
# tpx3serval_DBD += pycalcRecord.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from
# <name>.dbd
tpx3serval_SRCS += tpx3serval_registerRecordDeviceDriver.cpp
tpx3serval_SRCS += tpx3servalDriver.cpp
tpx3serval_SRCS += tpx3servalMain.cpp

# Add the support library
tpx3serval_LIBS += asyn

# Add the support library
tpx3serval_LIBS += dbRecStd
tpx3serval_LIBS += dbCore
tpx3serval_LIBS += ca
tpx3serval_LIBS += Com

# Build the main IOC entry point on workstation OSs.
tpx3serval_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#tpx3serval_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# tpx3serval_LIBS += pydev

# Finally link to the EPICS Base libraries
tpx3serval_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

