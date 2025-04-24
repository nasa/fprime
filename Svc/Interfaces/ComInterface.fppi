# ----------------------------------------------------------------------
# Com Data and Status
# ----------------------------------------------------------------------

@ Data coming in from the framing component
sync input port comDataIn: Svc.ComDataWithContext

@ Com data passing back out to the Driver
output port comDataOut: Drv.ByteStreamTransmit

@ Status of the last radio transmission
output port comStatus: Fw.SuccessCondition

# ----------------------------------------------------------------------
# Memory management
# ----------------------------------------------------------------------
# @ Buffer coming from a deallocate call in a ComDriver component
sync input port sentDataReturnIn: Drv.ByteStreamTransmit

output port bufferReturnOut: Svc.ComDataWithContext
