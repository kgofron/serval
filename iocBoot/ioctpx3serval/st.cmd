#!../../bin/linux-x86_64/tpx3serval

#- You may have to change tpx3serval to something else
#- everywhere it appears in this file

#< envPaths

## Register all support components
dbLoadDatabase "../../dbd/tpx3serval.dbd"
tpx3serval_registerRecordDeviceDriver(pdbbase) 

## Load record instances
dbLoadRecords("../../db/tpx3serval.db","P=TPX3-TEST:,R=Serval:,PORT=TPX3_PORT,ADDR=0,TIMEOUT=1.0")

## Configure the TPX3 serval driver
tpx3servalConfigure("TPX3_PORT", 1)

iocInit()

## Start any sequence programs
#seq snctpx3serval,"user=kg1"
