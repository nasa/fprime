# ----------------------------------------------------------------------
# Framing
# ----------------------------------------------------------------------
@ Port to receive data to frame, in a Fw::Buffer with optional context
guarded input port dataIn: Svc.ComDataWithContext

@ Port to output framed data, with optional context, for follow-up framing
output port framedDataOut: Svc.ComDataWithContext

# ----------------------------------------------------------------------
# Data ownership
# ----------------------------------------------------------------------
@ Port for returning ownership of the incoming Fw::Buffer to its sender
@ once framing is handled
output port dataReturn: Svc.ComDataWithContext

# ----------------------------------------------------------------------
# Handling of of ready signals
# ----------------------------------------------------------------------
@ Port receiving the general status from the downstream component
@ indicating it is ready or not-ready for more input
sync input port comStatusIn: Fw.SuccessCondition

@ Port receiving indicating the status of framer for receiving more data
output port comStatusOut: Fw.SuccessCondition
