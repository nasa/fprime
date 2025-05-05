# ----------------------------------------------------------------------
# Com Data and Status
# ----------------------------------------------------------------------

@ Data to be sent on the wire (coming in to the component)
sync input port comDataIn: Svc.ComDataWithContext

@ Data received from the wire (coming out of the component)
output port comDataOut: Fw.BufferSend

@ Status of the last transmission
output port comStatusOut: Fw.SuccessCondition

# ----------------------------------------------------------------------
# Memory management
# ----------------------------------------------------------------------

@ Returning ownership of data that came in on comDataIn
output port dataReturnOut: Svc.ComDataWithContext
