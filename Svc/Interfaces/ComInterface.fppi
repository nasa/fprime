# ----------------------------------------------------------------------
# Com Data and Status
# ----------------------------------------------------------------------

@ Data coming in to be sent out for transmission (e.g. from a Framer)
sync input port comDataIn: Svc.ComDataWithContext

@ Buffers sent out for transmission (e.g. to a driver)
output port comDataOut: Drv.ByteStreamData

@ Status of the last radio transmission
output port comStatusOut: Fw.SuccessCondition

# ----------------------------------------------------------------------
# Memory management
# ----------------------------------------------------------------------

@ Buffer ownership going back to sender - incoming from downstream driver
sync input port dataReturnIn: Drv.ByteStreamData

@ Buffer ownership going back to sender - outgoing upstream
output port dataReturnOut: Svc.ComDataWithContext
