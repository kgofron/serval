#!../../bin/linux-x86_64/tpx3serval

#- You may have to change tpx3serval to something else
#- everywhere it appears in this file

#< envPaths

## Register all support components
dbLoadDatabase "../../dbd/tpx3serval.dbd"
tpx3serval_registerRecordDeviceDriver(pdbbase) 

## Load record instances
#dbLoadRecords("../../db/tpx3serval.db","user=kg1")

iocInit()

## Start any sequence programs
#seq snctpx3serval,"user=kg1"
