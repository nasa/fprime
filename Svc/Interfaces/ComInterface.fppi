# ----------------------------------------------------------------------
# Com Data and Status
# ----------------------------------------------------------------------

@ Data to be sent on the wire (coming in to the component)
sync input port comDataIn: Svc.ComDataWithContext

@ Data received from the wire (coming out of the component)
output port comDataOut: Svc.ComDataWithContext

@ Status of the last transmission
output port comStatusOut: Fw.SuccessCondition

# ----------------------------------------------------------------------
# Memory management
# ----------------------------------------------------------------------

@ Port returning ownership of data that came in on comDataIn
output port dataReturnOut: Svc.ComDataWithContext

@ Port receiving back ownership of buffer sent out on comDataOut
sync input port bufferReturnIn: Fw.BufferSend
